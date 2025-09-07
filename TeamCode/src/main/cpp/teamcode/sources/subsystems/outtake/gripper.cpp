#include "subsystems/outtake/gripper.hpp"

#include "sdk.hpp"
#include "task_scheduler.hpp"
#include "subsystems/intake/gripper.hpp"
#include "subsystems/subsystem_utils.hpp"

using namespace sdk;
using namespace std;

namespace outtake::gripper
{
    Servo_impl_ex servo;

    Status status = Status::OPEN;
    bool moving = false;

    // TODO: Adjust this constant
    constexpr double OPEN_POSITION = 125_deg / servo_type::RANGE_OF_MOTION;
    // TODO: Adjust this constant
    constexpr double CLOSE_POSITION = OPEN_POSITION + 70_deg / servo_type::RANGE_OF_MOTION;
    constexpr double CLOSE_POSITION_LOSELY = CLOSE_POSITION - 22_deg / servo_type::RANGE_OF_MOTION;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        servo = hardware_map::get<Servo>("outtake_gripper");
        servo.set_pwm_range(servo_type::PWM_RANGE);
        servo.set_direction(Servo::Direction::REVERSE);
        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
    }

    bool grip(Status status, bool wait_until_done, bool open_intake_if_needed = true)
    {
        static double last_position = OPEN_POSITION;

        if (status == Status::OPEN)
            log(ANDROID_LOG_INFO, "outtake_gripper") << "opening gripper";
        else if (status == Status::CLOSED)
            log(ANDROID_LOG_INFO, "outtake_gripper") << "closing gripper";
        else
            log(ANDROID_LOG_INFO, "outtake_gripper") << "closing gripper loosely";

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
        task_scheduler::run_after([current_method_count, status, last_status, open_intake_if_needed]
                                  {
            if (current_method_count != method_count)
                return;

            if (status == Status::OPEN)
                log(ANDROID_LOG_INFO, "outtake_gripper") << "opening finished" << endl;
            else if (status == Status::CLOSED && last_status == Status::OPEN)
                log(ANDROID_LOG_INFO, "outtake_gripper") << "closing finished" << endl;
            else
                log(ANDROID_LOG_INFO, "outtake_gripper") << "closing loosely finished" << endl;

            if (gripper::status != Status::OPEN && last_status == Status::OPEN && open_intake_if_needed)
            {
                intake::gripper::open();
                task_scheduler::register_thread([] {
                    intake::gripper::wait_for_movement_finish();
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
        return grip(Status::OPEN, wait_until_done, true);
    }

    bool close(bool wait_until_done, bool open_intake_if_needed)
    {
        return grip(Status::CLOSED, wait_until_done, open_intake_if_needed);
    }

    bool close_losely(bool wait_until_done)
    {
        return grip(Status::CLOSED_LOSELY, wait_until_done, true);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }
} // gripper