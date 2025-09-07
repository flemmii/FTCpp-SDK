#include "subsystems/outtake/arm.hpp"

#include "sdk.hpp"
#include "utils/PID_controller.hpp"
#include "Units-master/Maths.h"
#include "subsystems/error_detection.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/outtake/elevator.hpp"
#include "subsystems/outtake/outtake.hpp"
#include "subsystems/outtake/wrist.hpp"
#include "subsystems/driving/pinpoint.hpp"
#include "subsystems/driving/drivetrain.hpp"

using namespace std;
using namespace sdk;

namespace outtake::arm
{
    // TODO: Adjust this constant
    constexpr Power MIN_MOTOR_POWER(0.13);
    constexpr AngularAcceleration DECELERATION(5000);
    constexpr Power DECELERATION_POWER(0.2);
    constexpr Degree MAX_DEVIATION(0.5);
    constexpr AngularSpeed MAX_SPEED_TO_START_CORRECTION = 5_deg / 1_s;
    PID_controller hold_pid(0.005, 0.0005, 0);

    const Degree MAX_ROTATION_EXTENSION_LIMIT = 90_deg + maths::Arcsin(MAX_REACH_EXTENSION_LIMIT / MAX_LENGTH);

    Dc_motor_ex motor;
    Analog_input absolute_encoder;

    Power max_motor_power = 1_power;
    Degree target_rotation = 0_deg;
    bool moving = false;
    size_t index;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        linkage::init();

        motor = hardware_map::get<Dc_motor_ex>("outtake_arm");
        motor.set_direction(Dc_motor_ex::Direction::REVERSE);
        absolute_encoder = hardware_map::get<Analog_input>("outtake_arm_encoder");

        index = error_detection::add_motor("arm", 3);

        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
        linkage::stop();
    }

    Millimetre get_length()
    {
        return BASE_LENGTH + linkage::target_extension;
    }

    Millimetre calc_reach(Degree rotation)
    {
        return get_length() * -maths::Cos(rotation);
    }

    Millimetre calc_height(Degree rotation)
    {
        return get_length() * maths::Sin(rotation);
    }

    Degree get_rotation()
    {
        return maths::Mod(Volt(absolute_encoder.get_voltage()) * DEG_PER_VOLT + OFFSET, 360_deg);
    }

    AngularSpeed get_rotation_speed()
    {
        return motor.get_velocity() * (DEG_PER_TICK * 1_tick) / 1_s;
    }

    void set_power(Power power)
    {
        static const function<void(double)> set_power = [](double power)
        {
            motor.set_power(power);
        };
        log(ANDROID_LOG_INFO, "outtake_arm") << "Setting power to " << power << endl;
        subsystem_utils::set_power<Degree>(set_power, get_rotation, power, MAX_ROTATION, MIN_ROTATION, method_count, moving, threads_waiting_for_movement_finish, moving_finished);
    }

    Power calc_anti_gravity_power()
    {
        static Speed last_speed_elevator = 0_m / 1_s;
        static chrono::steady_clock::time_point last_time = chrono::steady_clock::now() - 50ms;

        const Speed speed_elevator = elevator::get_elevation_speed();
        const chrono::steady_clock::time_point current_time = chrono::steady_clock::now();

        if (current_time - last_time > 50ms)
            last_speed_elevator = speed_elevator;

        const Degree rotation = get_rotation();
        const Power anti_gravity_power = subsystem_utils::conv_torque_to_power((((TORQUE_ON_ARM_AT_MAX_EXTENSION - TORQUE_ON_ARM_AT_ZERO_EXTENSION) * linkage::target_extension / linkage::MAX_EXTENSION) + TORQUE_ON_ARM_AT_ZERO_EXTENSION) *
                                                                                   (clamp(1 + ((speed_elevator - last_speed_elevator) / Millisecond(chrono::duration_cast<chrono::milliseconds>(current_time - last_time).count())) / GRAVITY, Real(0), Real(2)) * Real(maths::Cos(rotation)) +
                                                                                    clamp(pinpoint::conv_to_robot_centric(pinpoint::get_acceleration().to_vec()).forwards / GRAVITY, Real(-1), Real(1)) * -Real(maths::Sin(rotation))),
                                                                               0_deg / 1_s, GEAR_RATIO);

        last_speed_elevator = speed_elevator;
        last_time = current_time;

        return anti_gravity_power;
    }

    void hold_arm()
    {
        static const function<void(double)> set_power = [](double power)
        {
            motor.set_power(power);
        };

        log(ANDROID_LOG_INFO, "outtake_arm") << "Holding position" << endl;
        subsystem_utils::hold_position<Degree>(set_power, get_rotation, target_rotation, hold_pid, method_count, 100ms, calc_anti_gravity_power);
    }

    bool calc_and_set_power_rotate(const Degree &target_rotation, bool positive_power, Power &last_power, bool &started_deceleration, bool &started_correction)
    {
        Power power = subsystem_utils::go_to_position<Degree>(get_rotation, get_rotation_speed, target_rotation, MAX_DEVIATION, "outtake_arm", max_motor_power * (positive_power ? 1 : -1), MIN_MOTOR_POWER, DECELERATION_POWER, DECELERATION, MAX_SPEED_TO_START_CORRECTION, started_deceleration, started_correction);
        if (power != last_power)
        {
            clog << power << endl;
            last_power = power;
            motor.set_power(clamp(power + calc_anti_gravity_power(), -max_motor_power, max_motor_power).Value());
        }
        return power == 0_power; // someone was here
    }

    bool move_elevator_if_needed(Degree rotation)
    {
        const Millimetre height_diff = MIN_HEIGHT_ALLOWED - (calc_height(rotation) + elevator::target_elevation);
        if (height_diff > 0_mm)
            elevator::set_elevation(elevator::target_elevation + height_diff);

        return height_diff > 0_mm;
    }

    bool set_rotation(Degree rotation, bool wait_until_done)
    {
        if (rotation == target_rotation)
            return true;

        rotation = maths::Mod(rotation, 360_deg);

        if (rotation < MIN_ROTATION)
        {
            rotation += 360_deg;

            if (rotation > MAX_ROTATION)
                rotation = (MIN_ROTATION + 360_deg - rotation < rotation - MAX_ROTATION) ? MIN_ROTATION : MAX_ROTATION;
        }
        else if (rotation > MAX_ROTATION)
        {
            rotation -= 360_deg;

            if (rotation < MIN_ROTATION)
                rotation = (MIN_ROTATION - rotation < rotation + 360_deg - MAX_ROTATION) ? MIN_ROTATION : MAX_ROTATION;
        }

        if (rotation > MAX_ROTATION_EXTENSION_LIMIT)
            rotation = MAX_ROTATION_EXTENSION_LIMIT;

        log(ANDROID_LOG_INFO, "outtake_arm") << "Setting rotation to " << rotation << endl;

        target_rotation = rotation;
        const int current_method_count = ++method_count;
        moving = true;

        const Degree current_rotation = get_rotation();
        bool started_deceleration = false;
        bool started_correction = false;
        Power last_power = 0_power;
        const bool positive_power = rotation > current_rotation;

        task_scheduler::register_task(
            (function<bool()>)[=]() mutable {
                if (current_method_count != method_count)
                    return true;

                if (move_elevator_if_needed(rotation))
                    return false; // Elevator is moving, wait for it to start moving

                const Degree current_rotation = get_rotation();
                wrist::update_rotation(-current_rotation); // Hallo Florian hihi ich habe ein zweites hallo Florian geschrieben hihihihihi

                if (rotation < current_rotation)
                {
                    const Millimetre height_diff = MIN_HEIGHT_ALLOWED - (calc_height(current_rotation) + elevator::get_elevation());
                    // Otherwise the arm is allowed to freely rotate to the target rotation
                    if (height_diff > get_length())
                    {
                        const Degree temp_target_rotation = maths::Arcsin(height_diff / get_length());
                        // Check whether the temporary target rotation is not beyond the target rotation
                        if (temp_target_rotation > rotation)
                        {
                            bool trash_deceleration = true;
                            bool trash_correction = true;
                            calc_and_set_power_rotate(temp_target_rotation, positive_power, last_power, trash_deceleration, trash_correction);
                            return false; // Rotate to temporary target rotation first
                        }
                    }
                }

                // Rotate to the final target rotation
                return calc_and_set_power_rotate(rotation, positive_power, last_power, started_deceleration, started_correction);
            },
            30ms, true,
            [rotation, current_method_count]
            {
                if (current_method_count != method_count)
                    return;

                wrist::update_rotation(-rotation);

                log(ANDROID_LOG_INFO, "outtake_arm") << "finished rotating at " << get_rotation() << endl;

                hold_arm();

                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
            });

        if (wait_until_done)
            wait_for_movement_finish();

        return method_count == current_method_count;
    }

    bool move_to_transfer_position(bool wait_until_done)
    {
        return set_rotation(TRANSFER_ROTATION, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }

    namespace linkage
    {
        namespace servo_type = servos::plex;
        Servo_impl_ex servo;

        Millimetre target_extension = 0_mm;
        bool moving = false;

        int threads_waiting_for_movement_finish = 0;
        sem_t moving_finished;

        int method_count = 0;

        void init()
        {
            servo = hardware_map::get<Servo_impl_ex>("outtake_arm_linkage");
            servo.set_pwm_range(servo_type::PWM_RANGE);

            method_count = 0;

            threads_waiting_for_movement_finish = 0;
            sem_init(&moving_finished, 0, 0);
        }

        void stop()
        {
            sem_destroy(&moving_finished);
        }

        // rotation at 0mm extension
        const Degree zero_rotation = 180_deg - (maths::Arctan(ZERO_OFFSET.z, ZERO_OFFSET.y) +
                                                maths::Arccos((ZERO_OFFSET.y * ZERO_OFFSET.y + ZERO_OFFSET.z * ZERO_OFFSET.z + SERVO_ATTACHED_PART_LENGTH * SERVO_ATTACHED_PART_LENGTH - SLIDE_ATTACHED_PART_LENGTH * SLIDE_ATTACHED_PART_LENGTH) /
                                                              (2 * maths::Sqrt(ZERO_OFFSET.y * ZERO_OFFSET.y + ZERO_OFFSET.z * ZERO_OFFSET.z) * SERVO_ATTACHED_PART_LENGTH)));

        Degree last_rotation = zero_rotation;

        bool set_length(Millimetre length, bool wait_until_done)
        {
            length = clamp(length, 0_mm, MAX_EXTENSION);
            log(ANDROID_LOG_INFO, "outtake_arm_linkage") << "Setting extension to: " << length << endl;

            target_extension = length;
            const int current_method_count = ++method_count;

            // Change wrist rotation when wrist would interfere with arm linkage
            if (length > INTERFERENCE_EXTENSION_ARM_LINKAGE)
                wrist::update_rotation(wrist::coaxial_target_rotation);

            const Millimetre length_related_to_servo = ZERO_OFFSET.y + length;
            const Millimetre distance_servo_slide_linkage = maths::Sqrt(length_related_to_servo * length_related_to_servo + ZERO_OFFSET.z * ZERO_OFFSET.z);

            // rotation in relation to the mid position (90°)
            const Degree rotation = 180_deg - (maths::Arctan(ZERO_OFFSET.z, length_related_to_servo) +
                                               maths::Arccos((distance_servo_slide_linkage * distance_servo_slide_linkage + SERVO_ATTACHED_PART_LENGTH * SERVO_ATTACHED_PART_LENGTH - SLIDE_ATTACHED_PART_LENGTH * SLIDE_ATTACHED_PART_LENGTH) / (2 * distance_servo_slide_linkage * SERVO_ATTACHED_PART_LENGTH)));

            servo.set_position(clamp((0.5 + (rotation + OFFSET - (zero_rotation + ROTATION_OVERCENTERED) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_rotation) / MAX_VELOCITY).Value()));
            last_rotation = rotation;
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

                log(ANDROID_LOG_INFO, "outtake_arm_linkage") << "finished extending" << endl;
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished); },
                                      time_needed);

            if (wait_until_done)
                wait_for_movement_finish();

            return current_method_count == method_count;
        }

        bool retract(bool wait_until_done)
        {
            return set_length(0_mm, wait_until_done);
        }

        bool overcenter(bool wait_until_done)
        {
            log(ANDROID_LOG_INFO, "outtake_arm_linkage") << "Overcentering linkage" << endl;

            target_extension = MAX_EXTENSION;
            const int current_method_count = ++method_count;

            servo.set_position(clamp((0.5 + (ROTATION_OVERCENTERED + OFFSET - (zero_rotation + ROTATION_OVERCENTERED) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(ROTATION_OVERCENTERED - last_rotation) / MAX_VELOCITY).Value()));
            last_rotation = ROTATION_OVERCENTERED;
            if (time_needed == 0ms)
            {
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
                return current_method_count == method_count;
            }

            moving = true;
            const chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
            task_scheduler::run_after([current_method_count]
                                      {
                if (current_method_count != method_count)
                    return;

                servo.set_pwm_disable();
                log(ANDROID_LOG_INFO, "outtake_arm_linkage") << "finished overcentering" << endl;
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished); },
                                      time_needed);

            if (wait_until_done)
                wait_for_movement_finish();

            return current_method_count == method_count;
        }

        void wait_for_movement_finish()
        {
            subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
        }
    }
} // arm