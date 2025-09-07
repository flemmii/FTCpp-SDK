#pragma once

#include <semaphore.h>

#include "sdk.hpp"

#include "Units-master/Units.h"
#include "Units-master/Maths.h"
#include "constants.hpp"
#include "robot.hpp"
#include "match_data.hpp"
#include "task_scheduler.hpp"
#include "utils/PID_controller.hpp"

namespace subsystem_utils
{
    template <typename SpeedUnit, typename std::enable_if<units::IsUnit<SpeedUnit>::Value, int>::type = 0>
    constexpr typename Multiply<SpeedUnit, Second>::Type calc_distance_needed_to_stop(const SpeedUnit &speed, const typename Divide<SpeedUnit, Second>::Type &deceleration)
    {
        if (deceleration == typename Divide<SpeedUnit, Second>::Type(0))
            return typename Multiply<SpeedUnit, Second>::Type(0);

        return speed * abs(speed) / (2 * deceleration);
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    constexpr typename Divide<Unit, Second2>::Type calc_deceleration(const typename Divide<Unit, Second>::Type &speed, const Unit &distance_to_target)
    {
        if (distance_to_target == Unit(0))
            return typename Divide<Unit, Second2>::Type(0);

        return abs(speed * speed / (2 * distance_to_target));
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    constexpr void set_power(const function<void(double)> &set_power, const function<Unit()> &get_position, const Power &power, const Unit &max_position, const Unit &min_position, int &method_count, bool &moving, int &threads_waiting_for_movement_finish, sem_t &moving_finished)
    {
        if (match_data::current_match::match_number == 0)
        {
            Unit position = get_position();
            if ((position > max_position && power > 0_power) || (position < min_position && power < 0_power))
                return;
        }

        const int current_method_count = ++method_count;
        if (power != 0_power)
            moving = true;

        set_power(power.Value());

        if (power == 0_power)
        {
            moving = false;
            if (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished);

            return;
        }

        if (match_data::current_match::match_number != 0)
            return;

        task_scheduler::register_task((function<bool()>)[=] {
            const Unit position = get_position();
            return current_method_count != method_count ||
                   (position > max_position && power > 0_power) || (position < min_position && power < 0_power); },
                                      20ms, false,
                                      [set_power, get_position, max_position, min_position, &method_count, &moving, &threads_waiting_for_movement_finish, &moving_finished, current_method_count]
                                      {
                                          if (current_method_count != method_count)
                                              return;

                                          subsystem_utils::set_power(set_power, get_position, 0_power, max_position, min_position, method_count, moving, threads_waiting_for_movement_finish, moving_finished);
                                      });
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    constexpr void hold_position(const function<void(double)> &set_power, const function<Unit()> &get_position, const Unit &target_position, PID_controller &hold_pid, int &method_count, const chrono::steady_clock::duration &execution_interval, const function<Power()> &calc_anti_gravity_power = []()
                                                                                                                                                                                                                                                    { return 0_power; })
    {
        // Do not increase the method count as this only gets called by a internal method.
        // So we can be sure there is no other method running when this gets started.
        // If it's increased we have some problems as the method count is increased twice when this
        // internal method finishes (but only if the internal method calls this, so the method count only
        // gets increased twice sometimes)
        const int current_method_count = method_count;
        hold_pid.reset();
        task_scheduler::register_task((function<bool()>)[ set_power, get_position, target_position, &hold_pid, &method_count, calc_anti_gravity_power, current_method_count ] {
            const Unit current_position = get_position();

        if (current_method_count != method_count)
            return true;

        const Power power = Power(hold_pid.calculate_pid_algorithm((target_position - current_position).Value())) + calc_anti_gravity_power();

        set_power(power.Value()); 
        return false; }, execution_interval);
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    constexpr Power go_to_position(const function<Unit()> &get_position, const function<typename Divide<Unit, Second>::Type()> &get_speed, const Unit &target_position, const Unit &max_deviation, const string &tag, const Power &starting_power, const Power &min_motor_power, const Power &power_to_decelerate, const typename Divide<Unit, Second2>::Type &deceleration, const typename Divide<Unit, Second>::Type &max_speed_to_start_correction, bool &started_deceleration, bool &started_correction)
    {
        const Unit distance_to_target = target_position - get_position();

        Power power = starting_power;

        if (started_deceleration || (calc_distance_needed_to_stop(get_speed(), deceleration) * maths::Sgn(starting_power) > distance_to_target * maths::Sgn(starting_power)))
        {
            if (!started_deceleration)
            {
                log(ANDROID_LOG_INFO, tag) << "Starting controlled deceleration" << endl;
                started_deceleration = true;
            }

            if (started_correction || (get_speed() * maths::Sgn(starting_power) <= max_speed_to_start_correction * maths::Sgn(starting_power)))
            {
                started_correction = true;
                if (abs(distance_to_target) < max_deviation)
                    power = 0_power;
                else
                    power = min_motor_power * maths::Sgn(distance_to_target);
            }
            else
            {
                power = -power_to_decelerate * maths::Sgn(starting_power);
            }
        }

        return power;
    }

    constexpr void wait_for_movement_finish(const bool &moving, int &threads_waiting_for_movement_finish, sem_t &moving_finished)
    {
        if (!moving || sdk::linear_op_mode::is_stop_requested())
            return;

        threads_waiting_for_movement_finish++;
        sem_wait(&moving_finished);
        threads_waiting_for_movement_finish--;

        if (threads_waiting_for_movement_finish > 0)
            sem_post(&moving_finished);
    }

    constexpr void execute_function_or_register_thread(int &threads_running, const bool &wait_until_done, const std::function<void()> &function, const std::function<void()> &wait_function = [] {})
    {
        threads_running++;
        if (wait_until_done)
        {
            function();
            wait_function();
            threads_running--;
        }
        else
        {
            task_scheduler::register_thread([function, &threads_running]
                                            {
                function();
                threads_running--; });
        }
    }

    inline Power conv_torque_to_power(Torque torque, AngularSpeed rotational_speed, double gear_ratio)
    {
        static chrono::steady_clock::time_point last_time = chrono::steady_clock::now();
        static Volt voltage(12);
        if (chrono::steady_clock::now() - last_time > 500ms)
        {
            voltage = Volt(robot::control_hub.get_input_voltage(sdk::Voltage_unit::VOLTS));
            last_time = chrono::steady_clock::now();
        }

        const Power power_torque(torque * motors::RESISTANCE / (gear_ratio * motors::TORQUE_CONSTANT * voltage));
        return power_torque + abs(motors::COUNTER_ELECTROMOTIVE_FORCE * rotational_speed * gear_ratio / voltage) * 1_power * maths::Sgn(power_torque);
    }

    inline Torque conv_to_torque(Ampere current_draw, double gear_ratio)
    {
        return current_draw * motors::TORQUE_CONSTANT * gear_ratio;
    }
}