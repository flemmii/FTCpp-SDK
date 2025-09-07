#include "subsystems/intake/gripper.hpp"

#include "sdk.hpp"
#include "constants.hpp"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "match_data.hpp"
#include "subsystems/intake/arm.hpp"
#include "subsystems/outtake/gripper.hpp"

using namespace sdk;
using namespace std;

namespace intake::gripper
{
    Servo_impl_ex servo;
    Color_range_sensor color_range_sensor;

    Status status = Status::OPEN;
    bool closed = false;
    bool moving = false;

    // TODO: Adjust this constant
    constexpr double OPEN_POSITION = 110_deg / servo_type::RANGE_OF_MOTION;
    // TODO: Adjust this constant
    constexpr double CLOSE_POSITION_LOSELY = OPEN_POSITION - 63_deg / servo_type::RANGE_OF_MOTION;
    constexpr double CLOSE_POSITION = CLOSE_POSITION_LOSELY - 20_deg / servo_type::RANGE_OF_MOTION;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        swivel::init();

        servo = hardware_map::get<Servo>("intake_gripper");
        servo.set_pwm_range(servo_type::PWM_RANGE);
        color_range_sensor = hardware_map::get<Color_range_sensor>("intake_color_range");
        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
        swivel::stop();
    }

    bool grip(Status status, bool wait_until_done)
    {
        static double last_position = OPEN_POSITION;

        if (status == Status::OPEN)
            log(ANDROID_LOG_INFO, "intake_gripper") << "opening gripper";
        else if (status == Status::CLOSED)
            log(ANDROID_LOG_INFO, "intake_gripper") << "closing gripper";
        else
            log(ANDROID_LOG_INFO, "intake_gripper") << "closing gripper loosely";

        const int current_method_count = ++method_count;
        const Status last_status = gripper::status;
        gripper::status = status;

        double position;
        if (status == Status::OPEN)
            position = OPEN_POSITION;
        else if (status == Status::CLOSED)
            position = CLOSE_POSITION;
        else
            position = CLOSE_POSITION_LOSELY;

        servo.set_position(position);

        const auto time_needed = chrono::milliseconds(
            static_cast<int>(Millisecond(abs(last_position - position) * servo_type::RANGE_OF_MOTION /
                                         MAX_VELOCITY)
                                 .Value()));

        last_position = position;
        if (time_needed == 0ms)
        {
            moving = false;
            if (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished);
            return current_method_count == method_count;
        }

        moving = true;
        task_scheduler::run_after([current_method_count, status, last_status]
                                  {
            if (current_method_count != method_count)
                return;

            if (status == Status::OPEN)
                log(ANDROID_LOG_INFO, "intake_gripper") << "opening finished" << endl;
            else if (status == Status::CLOSED && last_status == Status::OPEN)
                log(ANDROID_LOG_INFO, "intake_gripper") << "closing finished" << endl;
            else
                log(ANDROID_LOG_INFO, "intake_gripper") << "closing loosely finished" << endl;

            if (gripper::status != Status::OPEN && last_status == Status::OPEN)
            {
                outtake::gripper::open();
                task_scheduler::register_thread([] {
                    outtake::gripper::wait_for_movement_finish();
                    moving = false;

                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished);
                });
            }
            else
            {
                moving = false;

                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
            } }, time_needed);

        if (wait_until_done)
            wait_for_movement_finish();

        return method_count == current_method_count;
    }

    bool open(bool wait_until_done)
    {
        closed = false;
        return grip(Status::OPEN, wait_until_done);
    }

    bool close_losely(bool wait_until_done)
    {
        closed = true;
        return grip(Status::CLOSED_LOSELY, wait_until_done);
    }

    bool close(bool wait_until_done)
    {
        closed = true;
        return grip(Status::CLOSED, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }

    constexpr Millimetre DISTANCE_TO_DETECT_SAMPLE = 30_mm;
    bool correct_sample()
    {
        Sample_color color = get_sample_color();
        return color == Sample_color::YELLOW || (color == Sample_color::BLUE && !match_data::current_match::red_alliance) || (color == Sample_color::RED && match_data::current_match::red_alliance);
    }

    Sample_color get_sample_color()
    {
        const Millimetre distance = Millimetre(color_range_sensor.get_distance(Distance_unit::MM));
        log(ANDROID_LOG_DEBUG, "DEBUG") << "distance: " << distance << endl;
        if (distance > DISTANCE_TO_DETECT_SAMPLE)
            return Sample_color::NONE;

        const double red = color_range_sensor.red();
        const double green = color_range_sensor.green();
        const double blue = color_range_sensor.blue();
        log(ANDROID_LOG_DEBUG, "DEBUG") << "red: " << red << endl;
        log(ANDROID_LOG_DEBUG, "DEBUG") << "green: " << green << endl;
        log(ANDROID_LOG_DEBUG, "DEBUG") << "blue: " << blue << endl;
        if (green > red && green > blue)
            return Sample_color::YELLOW;
        if (red > blue)
            return Sample_color::RED;

        return Sample_color::BLUE;
    }

    namespace swivel
    {
        Servo_impl_ex servo;

        Degree manual_set_target_rotation = 0_deg;
        Degree coaxial_target_rotation = 0_deg;
        bool moving = false;

        int threads_waiting_for_movement_finish = 0;
        sem_t moving_finished;

        int method_count = 0;

        void init()
        {
            servo = hardware_map::get<Servo>("intake_gripper_swivel");
            servo.set_pwm_range(servo_type::PWM_RANGE);

            method_count = 0;

            threads_waiting_for_movement_finish = 0;
            sem_init(&moving_finished, 0, 0);
        }

        void stop()
        {
            sem_destroy(&moving_finished);
        }

        bool coaxial_mode = true;

        bool rotate(Degree manual_rotation, Degree coaxial_rotation, bool wait_until_done)
        {
            Degree rotation = manual_rotation + coaxial_rotation;

            if (rotation < MIN_ROTATION)
                rotation = MAX_ROTATION - (MIN_ROTATION - rotation);
            else if (rotation > MAX_ROTATION)
                rotation = MIN_ROTATION + (rotation - MAX_ROTATION);

            log(ANDROID_LOG_INFO, "intake_gripper_swivel") << "Setting rotation to manual: " << manual_rotation << " coaxial: " << coaxial_rotation << endl;

            const int current_method_count = ++method_count;
            const Degree last_target_rotation = manual_set_target_rotation + coaxial_target_rotation;
            manual_set_target_rotation = manual_rotation;
            coaxial_target_rotation = coaxial_rotation;

            servo.set_position(clamp((0.5 + (rotation + OFFSET - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_target_rotation) / MAX_VELOCITY).Value()));
            if (time_needed == 0ms)
            {
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
                return current_method_count == method_count;
            }

            moving = true;
            const chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
            task_scheduler::run_after([current_method_count, rotation]
                                      {

                if (current_method_count != method_count)
                    return;

                log(ANDROID_LOG_INFO, "intake_gripper_swivel") << "finished rotating" << endl;
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished); },
                                      time_needed);

            if (wait_until_done)
                wait_for_movement_finish();

            return current_method_count == method_count;
        }

        bool update_rotation(Degree coaxial_rotation, bool wait_until_done)
        {
            if (coaxial_mode)
                return rotate(manual_set_target_rotation, coaxial_rotation, wait_until_done);
            else
                return true;
        }

        bool set_rotation(Degree rotation, bool wait_until_done, optional<Degree> coaxial_rotation)
        {
            coaxial_mode = !coaxial_rotation.has_value();
            return rotate(rotation, coaxial_rotation.has_value() ? coaxial_rotation.value() : coaxial_target_rotation, wait_until_done);
        }

        bool rotate_to_transfer(bool wait_until_done)
        {
            return set_rotation(0_deg, wait_until_done);
        }

        void wait_for_movement_finish()
        {
            subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
        }

        bool ready_for_transfer()
        {
            return (manual_set_target_rotation + coaxial_target_rotation) == 0_deg && !moving;
        }
    } // swivel
} // intake_gripper