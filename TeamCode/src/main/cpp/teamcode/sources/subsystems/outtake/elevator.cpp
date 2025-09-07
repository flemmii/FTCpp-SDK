#include "subsystems/outtake/elevator.hpp"

#include "sdk.hpp"
#include "utils/PID_controller.hpp"
#include "subsystems/error_detection.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/outtake/arm.hpp"
#include "subsystems/outtake/outtake.hpp"

using namespace std;
using namespace sdk;

namespace outtake::elevator
{
    // TODO: Adjust this constant
    constexpr Power MIN_MOTOR_POWER(0.3);
    constexpr ::Acceleration DECELERATION(40);
    constexpr Power DECELERATION_POWER(1);
    constexpr Millimetre MAX_DEVIATION = 2_mm;
    constexpr Speed MAX_SPEED_TO_START_CORRECTION = 50_mm / 1_s;
    PID_controller hold_pid(0.0028, 0.00028, 0);

    Dc_motor_ex right_motor;
    Dc_motor_ex left_motor;
    Touch_sensor homing_switch;

    Power max_motor_power = 1_power;
    Millimetre target_elevation = 0_mm;
    bool moving = false;
    size_t index;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        right_motor = hardware_map::get<Dc_motor_ex>("outtake_right_elevator");
        right_motor.set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
        left_motor = hardware_map::get<Dc_motor_ex>("outtake_left_elevator");
        left_motor.set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
        left_motor.set_direction(Dc_motor_ex::Direction::REVERSE);
        left_motor.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
        left_motor.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
        homing_switch = hardware_map::get<Touch_sensor>("outtake_elevator_homing");

        index = error_detection::add_motor("elevator", 3);

        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
    }

    Millimetre get_length()
    {
        return Tick(left_motor.get_current_position()) * MM_PER_TICK;
    }

    Speed get_speed()
    {
        return left_motor.get_velocity() * (MM_PER_TICK * 1_tick) / 1_s;
    }

    Millimetre get_elevation()
    {
        return conv_to_elevation(get_length());
    }

    Speed get_elevation_speed()
    {
        return conv_to_elevation<Second>(get_speed());
    }

    Power get_power()
    {
        return Power(left_motor.get_power());
    }

    bool homed()
    {
        return homing_switch.is_pressed();
    }

    void set_power_override(Power power)
    {
        log(ANDROID_LOG_INFO, "outtake_elevator") << "Setting power override to " << power << endl;

        const int current_method_count = ++method_count;
        if (power != 0_power)
            moving = true;

        right_motor.set_power(power.Value());
        left_motor.set_power(power.Value());

        if (power == 0_power)
        {
            moving = false;
            if (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished);

            return;
        }
    }

    void set_power(Power power)
    {
        static const function<void(double)> set_power = [](double power)
        {
            right_motor.set_power(power);
            left_motor.set_power(power);
        };
        log(ANDROID_LOG_INFO, "outtake_elevator") << "Setting power to " << power << endl;
        subsystem_utils::set_power<Millimetre>(set_power, get_elevation, power, MAX_ELEVATION, 0_mm, method_count, moving, threads_waiting_for_movement_finish, moving_finished);
    }

    bool reset_position(bool wait_until_done)
    {
        log(ANDROID_LOG_INFO, "outtake_elevator") << "Resetting position" << endl;

        target_elevation = 0_mm;
        const int current_method_count = ++method_count;
        moving = true;
        static constexpr Power power(0.3);

        right_motor.set_power(power.Value());
        left_motor.set_power(power.Value());

        task_scheduler::register_task((function<bool()>)[current_method_count] { return !homed() || current_method_count != method_count; }, 50ms, false, [current_method_count]
                                      {
            if (current_method_count != method_count)
                return;

            task_scheduler::run_after([current_method_count] {
                if (current_method_count != method_count)
                    return;

                right_motor.set_power(-power.Value());
                left_motor.set_power(-power.Value());

                task_scheduler::register_task((function<bool()>) [current_method_count] {
                    return homed() || current_method_count != method_count;
                }, 50ms, false, [current_method_count] {
                    if (current_method_count != method_count)
                        return;

                    right_motor.set_power(0);
                    left_motor.set_power(0);

                    task_scheduler::run_after([current_method_count] {
                        if (current_method_count != method_count)
                            return;
                    left_motor.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
                    left_motor.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);

                    if (arm::target_rotation == 0_deg && !arm::moving)
                        arm::set_power(0_power);

                    log(ANDROID_LOG_INFO, "outtake_elevator") << "Resetting finished" << endl;

                    moving = false;

                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished);
                    }, 100ms);
                });
            }, 200ms); });

        if (wait_until_done)
            wait_for_movement_finish();

        return method_count == current_method_count;
    }

    bool home(bool wait_until_done)
    {
        log(ANDROID_LOG_INFO, "outtake_elevator") << "Homing elevator" << endl;

        target_elevation = 0_mm;
        const int current_method_count = ++method_count;
        moving = true;
        static constexpr Power power(-0.3);

        right_motor.set_power(power.Value());
        left_motor.set_power(power.Value());

        task_scheduler::register_task((function<bool()>)[current_method_count] { return homed() || current_method_count != method_count; }, 50ms, false, [current_method_count]
                                      {
            if (current_method_count != method_count)
                return;

            right_motor.set_power(0);
            left_motor.set_power(0);

            if (arm::target_rotation == 0_deg && !arm::moving)
                arm::set_power(0_power);

            log(ANDROID_LOG_INFO, "outtake_elevator") << "Homing finished" << endl;

            moving = false;

            if (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished); });

        if (wait_until_done)
            wait_for_movement_finish();

        return method_count == current_method_count;
    }

    Kilogram get_mass()
    {
        return MASS + clamp(floor(get_length() / 180_mm), 0.0, 3.0) * (slides::MASS);
    }

    Power calc_anti_gravity_power(bool hang)
    {
        const Newton force = ((hang ? -robot::MASS : (get_mass() * maths::Cos(ANGLE))) * GRAVITY);
        return subsystem_utils::conv_torque_to_power(force * SPOOL_DIAMETER / 2, 0_deg / 1_s, GEAR_RATIO) / 2;
    }

    void calc_and_set_power_hold(bool hang)
    {
        const Millimetre current_elevation = get_elevation();

        Power power = Power(hold_pid.calculate_pid_algorithm(Millimetre(target_elevation - current_elevation).Value())) + calc_anti_gravity_power(hang);

        right_motor.set_power(power.Value());
        left_motor.set_power(power.Value());
    }

    void hold_elevator(bool hang)
    {
        log(ANDROID_LOG_INFO, "outtake_elevator") << "Holding elevator" << endl;
        // Do not increase the method count as this only gets called by a internal method.
        // So we can be sure there is no other method running when this gets started.
        // If it's increased we have some problems as the method count is increased twice when this
        // internal method finishes (but only if the internal method calls this, so the method count only
        // gets increased twice sometimes)
        const int current_method_count = method_count;

        hold_pid.reset();

        task_scheduler::register_task((function<bool()>)[ current_method_count, hang ]() mutable {
            if (current_method_count != method_count)
                return true;

            calc_and_set_power_hold(hang);

            return false;
        },
                                      100ms, true);
    }

    bool calc_and_set_power_elevate(const Millimetre &target_elevation, bool hang, bool positive_power, Power &last_power, bool &started_deceleration, bool &started_correction)
    {
        if (target_elevation == 0_mm)
        {
            if (last_power != -max_motor_power)
            {
                last_power = -max_motor_power;
                right_motor.set_power(-max_motor_power.Value());
                left_motor.set_power(-max_motor_power.Value());
            }
            return homed();
        }

        Power power = subsystem_utils::go_to_position<Millimetre>(get_elevation, get_elevation_speed, target_elevation, MAX_DEVIATION, "outtake_elevator", max_motor_power * (positive_power ? 1 : -1), MIN_MOTOR_POWER, DECELERATION_POWER, DECELERATION, MAX_SPEED_TO_START_CORRECTION, started_deceleration, started_correction);
        const Power total_power = clamp(power + calc_anti_gravity_power(hang), -max_motor_power, max_motor_power);
        if (last_power != total_power)
        {
            last_power = total_power;
            right_motor.set_power(total_power.Value());
            left_motor.set_power(total_power.Value());
        }
        return power == 0_power;
    }

    bool change_height_if_needed(Millimetre &target_elevation)
    {
        const Millimetre height_diff = MIN_HEIGHT_ALLOWED - (arm::calc_height(arm::target_rotation) + target_elevation);
        if (height_diff > 0_mm)
        {
            target_elevation = height_diff;
            return true;
        }

        return false;
    }

    bool set_elevation(Millimetre mm, bool wait_until_done, bool hang)
    {
        if (mm == target_elevation)
            return true;

        change_height_if_needed(mm);

        mm = clamp(mm, 0_mm, MAX_ELEVATION);

        if (hang)
            log(ANDROID_LOG_INFO, "outtake_elevator") << "Setting elevation to " << mm << " in hang mode" << endl;
        else
            log(ANDROID_LOG_INFO, "outtake_elevator") << "Setting elevation to " << mm << " in normal mode" << endl;

        target_elevation = mm;
        const int current_method_count = ++method_count;
        moving = true;

        const Millimetre current_elevation = get_elevation();
        bool started_deceleration = false;
        bool started_correction = false;
        Power last_power = 0_power;
        const bool positive_power = mm > current_elevation;

        // Ensuring that the arm is holding its position
        if (mm != 0_mm)
            arm::set_rotation(arm::target_rotation);

        task_scheduler::register_task(
            (function<
                bool()>)[=]() mutable {
                if (current_method_count != method_count)
                    return true;

                if (change_height_if_needed(mm))
                {
                    set_elevation(mm);
                    return true;
                }

                const Millimetre current_elevation = get_elevation();

                // Arm must be moving and the elevator must be moving downwards
                if (arm::moving && mm < current_elevation)
                {
                    const Degree current_rotation = arm::get_rotation();
                    // Arm must be moving upwards
                    if (arm::target_rotation > current_rotation)
                    {
                        const Millimetre height_diff = MIN_HEIGHT_ALLOWED - (arm::calc_height(current_rotation) + mm);
                        // Check whether the arm would collide with the robot with the current target elevation
                        if (height_diff > 0_mm)
                        {
                            bool trash_deceleration = true;
                            bool trash_correction = true;
                            calc_and_set_power_elevate(mm + height_diff, hang, positive_power, last_power, trash_deceleration, trash_correction);
                            return false;
                        }
                    }
                }

                return calc_and_set_power_elevate(mm, hang, positive_power, last_power, started_deceleration, started_correction);
            },
            30ms, true,
            [mm, current_method_count, hang]
            {
                if (current_method_count != method_count)
                    return;

                log(ANDROID_LOG_INFO, "outtake_elevator") << "finished elevating at " << get_elevation() << endl;

                if (hang || mm != 0_mm)
                {
                    hold_elevator(hang);
                }
                else
                {
                    if (abs(get_elevation()) > 10_mm)
                    {
                        reset_position();
                        return;
                    }
                    else
                    {
                        right_motor.set_power(0);
                        left_motor.set_power(0);
                    }
                }

                if (mm == 0_mm && arm::target_rotation == 0_deg && !arm::moving)
                    arm::set_power(0_power);

                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
            });
        if (wait_until_done)
            wait_for_movement_finish();

        return method_count == current_method_count;
    }

    bool retract(bool wait_until_done)
    {
        return set_elevation(0_mm, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }
} // elevator