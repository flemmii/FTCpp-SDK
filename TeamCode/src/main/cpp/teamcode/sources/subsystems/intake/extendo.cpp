#include "subsystems/intake/extendo.hpp"

#include "sdk.hpp"
#include "utils/PID_controller.hpp"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/error_detection.hpp"

using namespace std;
using namespace sdk;

namespace intake::extendo
{
    // TODO: Adjust this constant
    constexpr Power MIN_MOTOR_POWER(0.4);
    constexpr ::Acceleration DECELERATION(10);
    constexpr Power DECELERATION_POWER(0.3);
    constexpr Millimetre MAX_DEVIATION = 10_mm;
    constexpr Speed MAX_SPEED_TO_START_CORRECTION = 50_mm / 1_s;
    PID_controller hold_pid(0.0082, 0.00041, 0);

    Dc_motor_ex motor;
    Touch_sensor homing_switch;

    Power max_motor_power = 1_power;
    Millimetre target_extension = 0_mm;
    bool moving = false;
    size_t index;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        motor = hardware_map::get<Dc_motor_ex>("intake_extendo");
        motor.set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
        motor.set_direction(Dc_motor_ex::Direction::REVERSE);
        homing_switch = hardware_map::get<Touch_sensor>("intake_extendo_homing");

        index = error_detection::add_motor("extendo", 3);

        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
    }

    Millimetre get_extension()
    {
        return Tick(motor.get_current_position()) * MM_PER_TICK;
    }

    Speed get_extension_speed()
    {
        return motor.get_velocity() * (MM_PER_TICK * 1_tick) / 1_s;
    }

    bool homed()
    {
        return homing_switch.is_pressed();
    }

    void set_power(Power power)
    {
        static const function<void(double)> set_power = [](double power)
        {
            motor.set_power(power);
        };
        log(ANDROID_LOG_INFO, "intake_extendo") << "Setting power to " << power;
        subsystem_utils::set_power<Millimetre>(set_power, get_extension, power, MAX_EXTENSION, 0_mm, method_count, moving, threads_waiting_for_movement_finish, moving_finished);
    }

    bool reset_position(bool wait_until_done)
    {
        log(ANDROID_LOG_INFO, "intake_extendo") << "Resetting position";

        target_extension = 0_mm;
        const int current_method_count = ++method_count;
        moving = true;
        static constexpr Power power(0.6);

        motor.set_power(power.Value());

        task_scheduler::register_task((function<bool()>)[current_method_count] { return !homed() || current_method_count != method_count; }, 50ms, false, [current_method_count]
                                      {
            if (current_method_count != method_count)
                return;

            task_scheduler::run_after([current_method_count] {
                if (current_method_count != method_count)
                    return;

                motor.set_power(-power.Value());

                task_scheduler::register_task((function<bool()>) [current_method_count] {
                    return homed() || current_method_count != method_count;
                }, 50ms, false, [current_method_count] {
                    if (current_method_count != method_count)
                        return;

                    motor.set_power(0);

                    task_scheduler::run_after([current_method_count] {
                        if (current_method_count != method_count)
                            return;
                    motor.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
                    motor.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);

                    log(ANDROID_LOG_INFO, "intake_extendo") << "Resetting finished";

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
        log(ANDROID_LOG_INFO, "intake_extendo") << "Homing extendo";

        target_extension = 0_mm;
        const int current_method_count = ++method_count;
        moving = true;

        motor.set_power(-0.3);

        task_scheduler::run_after([current_method_count]
                                  { task_scheduler::register_task((function<bool()>)[current_method_count] { return homed() || current_method_count != method_count; }, 50ms, false, [current_method_count]
                                                                  {
                                      if (current_method_count != method_count)
                                          return;

                                      motor.set_power(0);

                                      log(ANDROID_LOG_INFO, "intake_extendo") << "Homing finished";

                                      moving = false;

                                      if (threads_waiting_for_movement_finish > 0)
                                          sem_post(&moving_finished); }); }, 200ms);

        if (wait_until_done)
            wait_for_movement_finish();

        return method_count == current_method_count;
    }

    Power calc_anti_gravity_power_hang()
    {
        return subsystem_utils::conv_torque_to_power(-robot::MASS * GRAVITY * SPOOL_DIAMETER_HANG / 2, 0_deg / 1_s, GEAR_RATIO * GEAR_RATIO_MULTIPLIER_HANG);
    }

    void hold_extendo(bool hang)
    {
        static const function<void(double)> set_power = [](double power)
        {
            motor.set_power(power);
        };
        log(ANDROID_LOG_INFO, "intake_extendo") << "Holding position";

        subsystem_utils::hold_position<Millimetre>(set_power, get_extension, target_extension, hold_pid, method_count, 100ms, hang ? calc_anti_gravity_power_hang : ([]
                                                                                                                                                                     { return 0_power; }));
    }

    bool set_extension(Millimetre mm, bool wait_until_done, bool hang)
    {
        if (mm == target_extension)
            return true;

        mm = clamp(mm, 0_mm, MAX_EXTENSION);

        log(ANDROID_LOG_INFO, "intake_extendo") << "Setting extension to " << mm << endl;

        target_extension = mm;
        const int current_method_count = ++method_count;
        moving = true;

        const Millimetre current_extension = get_extension();
        bool started_deceleration = false;
        bool started_correction = false;
        Power last_power = 0_power;
        const bool positive_power = mm > current_extension;

        task_scheduler::register_task(
            (function<bool()>)[=]() mutable {
                if (current_method_count != method_count)
                    return true;

                if (target_extension == 0_mm)
                {
                    if (last_power != -max_motor_power)
                    {
                        last_power = -max_motor_power;
                        motor.set_power(-max_motor_power.Value());
                    }
                    return homed();
                }

                Power power = subsystem_utils::go_to_position<Millimetre>(get_extension, get_extension_speed, mm, MAX_DEVIATION, "intake_extendo", max_motor_power * (positive_power ? 1 : -1), MIN_MOTOR_POWER, DECELERATION_POWER, DECELERATION, MAX_SPEED_TO_START_CORRECTION, started_deceleration, started_correction);
                if (last_power != power)
                {
                    last_power = power;
                    motor.set_power(clamp(power + (hang ? calc_anti_gravity_power_hang() : 0_power), -max_motor_power, max_motor_power).Value());
                }
                return power == 0_power;
            },
            30ms, true,
            [mm, current_method_count, hang]
            {
                if (current_method_count != method_count)
                    return;

                log(ANDROID_LOG_INFO, "intake_extendo") << "finished extending at " << get_extension() << endl;

                if (mm == 0_mm && !hang)
                {
                    if (abs(get_extension()) > 10_mm)
                    {
                        reset_position();
                        return;
                    }
                    else
                    {
                        motor.set_power(0);
                    }

                    if (method_count != current_method_count)
                        return;
                }
                else
                {
                    hold_extendo(hang);
                }

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
        return set_extension(0_mm, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }

    bool ready_for_transfer()
    {
        return target_extension == 0_mm && !moving;
    }
} // extendo