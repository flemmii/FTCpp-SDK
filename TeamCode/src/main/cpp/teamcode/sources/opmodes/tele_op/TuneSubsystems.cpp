#include "opmodes/tele_op/TuneSubsystems.hpp"

#include "opmodes/include_all_header.hpp"

using namespace std;

using namespace ::gamepads;

using namespace sdk;
using namespace linear_op_mode;

/*
namespace TuneSubsystems
{
    Power calc_anti_gravity_power_elevator()
    {
        return subsystem_utils::conv_torque_to_power(outtake::elevator::get_mass() * ::Acceleration(9.81) * outtake::elevator::SPOOL_DIAMETER / 2, 0_deg / 1_s, outtake::elevator::GEAR_RATIO) / 2;
    }

    Power calc_anti_gravity_power_arm()
    {
        return subsystem_utils::conv_torque_to_power(outtake::arm::B * ::Acceleration(9.81) * maths::Cos(outtake::arm::get_rotation()) * outtake::arm::LEVER_ARM, 0_deg / 1_s, outtake::arm::GEAR_RATIO);
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    void do_min_motor_power_tuning(const function<Unit()> &get_position, const function<void(Power)> &set_power, const function<Power()> &calc_anti_gravity_power, const Unit &upper_limit, const Unit &lower_limit, Power &min_motor_power)
    {
        bool reverse_power = false;
        constexpr Power power_per_loop(0.001);
        robot::control_hub.get_bulk_data();
        robot::expansion_hub.get_bulk_data();
        Unit last_position = get_position();

        bool touched_lower_limit;
        bool touched_upper_limit;
        bool started_reliability_test;
        while (!linear_op_mode::is_stop_requested())
        {
            set_power(min_motor_power + calc_anti_gravity_power());
            sleep(500ms);
            robot::control_hub.get_bulk_data();
            robot::expansion_hub.get_bulk_data();
            const Unit current_position = get_position();

            if (current_position > upper_limit && !reverse_power)
            {
                if (touched_lower_limit && touched_upper_limit)
                    break;

                if (started_reliability_test)
                    touched_upper_limit = true;
                min_motor_power = -min_motor_power;
                reverse_power = true;
            }
            else if (current_position < lower_limit && reverse_power)
            {
                if (touched_lower_limit && touched_upper_limit)
                    break;

                if (started_reliability_test)
                    touched_lower_limit = true;
                min_motor_power = -min_motor_power;
                reverse_power = false;
            }

            if ((current_position - last_position) != Unit(0))
            {
                if (!started_reliability_test)
                {
                    started_reliability_test = true;
                    touched_lower_limit = false;
                    touched_upper_limit = false;
                }
            }
            else
            {
                started_reliability_test = false;
                min_motor_power += reverse_power ? -power_per_loop : power_per_loop;
            }

            last_position = current_position;
            telemetry::add_data("min motor power", to_string(min_motor_power));
            telemetry::update();
        }
        min_motor_power = abs(min_motor_power);
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    void do_deceleration_tuning(const function<Unit()> &get_position, const function<typename Divide<Unit, Second>::Type()> &get_speed, const function<void(Power)> &set_power, const function<Power()> &calc_anti_gravity_power, const Power &max_power, const Unit &upper_limit, const Unit &lower_limit, typename Divide<Unit, Second2>::Type &deceleration)
    {
        robot::control_hub.get_bulk_data();
        robot::expansion_hub.get_bulk_data();
        constexpr int loops = 1;
        typename Divide<Unit, Second2>::Type decelerations_added_up(0);

        for (int i = 0; i < loops; i++)
        {
            int reverse = (get_position() < (upper_limit + lower_limit) / 2) ? 1 : -1;
            set_power(-max_power / 3 * reverse);
            while (get_position() * reverse > (reverse == 1 ? lower_limit : upper_limit) * reverse)
            {
                robot::control_hub.get_bulk_data();
                robot::expansion_hub.get_bulk_data();
            }
            set_power(0_power);
            sleep(1s);

            set_power(max_power * reverse + calc_anti_gravity_power());
            while (get_position() * reverse < (upper_limit + lower_limit) / 2 * reverse)
            {
                robot::control_hub.get_bulk_data();
                robot::expansion_hub.get_bulk_data();
            }

            chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
            typename Divide<Unit, Second>::Type start_speed = get_speed();

            set_power(-max_power * reverse + calc_anti_gravity_power());
            while (get_speed() * reverse > typename Divide<Unit, Second>::Type(0))
            {
                robot::control_hub.get_bulk_data();
                robot::expansion_hub.get_bulk_data();
            }
            set_power(0_power);

            decelerations_added_up += abs(start_speed) / Millisecond(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count());
        }
        deceleration = decelerations_added_up / loops;

        telemetry::add_data("deceleration", to_string(deceleration));
        telemetry::update();
    }

    template <typename Unit, typename std::enable_if<units::IsUnit<Unit>::Value, int>::type = 0>
    void do_correction_tuning(const function<Unit()> &get_position, const function<typename Divide<Unit, Second>::Type()> &get_speed, const function<void(Power)> &set_power, const function<Power()> &calc_anti_gravity_power, const Power &max_power, const Unit &upper_limit, const Unit &lower_limit, const Unit &max_deviation, const Power &min_motor_power, typename Divide<Power, Unit>::Type &correction_proportional, typename Divide<Unit, Second2>::Type deceleration, const Unit &max_overshoot_allowed, const tuple<Power, Unit> &targeted_correction_power_at_specified_distance, const chrono::steady_clock::duration &execution_interval)
    {
        const Unit distance = get<1>(targeted_correction_power_at_specified_distance);
        const auto steps_p = ((get<0>(targeted_correction_power_at_specified_distance)) / (distance * distance)) / 10;

        Power starting_power = max_power;

        int reliability_test_count = 0;

        while (!linear_op_mode::is_stop_requested())
        {
            robot::control_hub.get_bulk_data();
            robot::expansion_hub.get_bulk_data();

            const Unit target = (get_position() < (upper_limit + lower_limit) / 2 ? upper_limit : lower_limit);
            starting_power = (target == upper_limit ? 1 : -1) * abs(starting_power);
            set_power(starting_power + calc_anti_gravity_power());

            bool started_deceleration = false;
            Unit max_overshoot(0);

            chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
            while (!linear_op_mode::is_stop_requested())
            {
                robot::control_hub.get_bulk_data();
                robot::expansion_hub.get_bulk_data();

                const Unit position = get_position();
                const typename Divide<Unit, Second>::Type speed = get_speed();
                const Unit distance_needed_to_stop = subsystem_utils::calc_distance_needed_to_stop(speed, deceleration);
                const Unit distance_to_target = target - position;
                if ((distance_needed_to_stop * maths::Sgn(distance_to_target) > abs(distance_to_target)) || started_deceleration)
                {
                    started_deceleration = true;
                    max_overshoot = max(max_overshoot, distance_to_target * (target == upper_limit ? -1 : 1));
                    Unit error = distance_to_target - distance_needed_to_stop;

                    if (get_speed() * (target == upper_limit ? 1 : -1) <= typename Divide<Unit, Second>::Type(0))
                    {
                        set_power(calc_anti_gravity_power());
                        break;
                    }
                    else
                    {
                        set_power(-starting_power + calc_anti_gravity_power() + error * correction_proportional);
                    }
                }
                else
                {
                    set_power(starting_power + calc_anti_gravity_power());
                }
                sleep(execution_interval);
            }

            if (max_overshoot <= max_overshoot_allowed)
            {
                if (reliability_test_count++ > 10)
                    break;
            }
            else
            {
                reliability_test_count = 0;
                correction_proportional += max_overshoot * steps_p;
            }

            telemetry::add_data("min motor power", to_string(min_motor_power));
            telemetry::add_data("deceleration", to_string(deceleration));
            telemetry::add_data("correction proportional", to_string(correction_proportional));
            telemetry::update();
        }
    }

    extern "C" void TuneSubsystems()
    {
        robot::init();
        outtake::elevator::auto_detect_hang = false;

        gamepad1.a.add_press_listener([]
                                      {
            constexpr bool tune_min_motor_power = false;
            constexpr bool tune_deceleration = true;
            constexpr bool tune_pd = false;


            Power min_motor_power(0.261);
            if constexpr (tune_min_motor_power)
                do_min_motor_power_tuning<Millimetre>(intake::extendo::get_extension, intake::extendo::set_power, []() { return 0_power; }, 400_mm, 100_mm, min_motor_power);

            constexpr Millimetre upper_limit = 400_mm;
            constexpr Millimetre lower_limit = 100_mm;

            ::Acceleration deceleration(0);
            constexpr Millimetre MAX_DEVIATION = 5_mm;
            if constexpr (tune_deceleration)
                do_deceleration_tuning<Metre>(intake::extendo::get_extension, intake::extendo::get_extension_speed, intake::extendo::set_power, []() { return 0_power; }, 1_power, upper_limit, lower_limit, deceleration);

            Divide<Power, Metre>::Type extend_proportinal(0);
            if constexpr (tune_pd)
                do_correction_tuning<Metre>(intake::extendo::get_extension, intake::extendo::get_extension_speed, intake::extendo::set_power, []() { return 0_power; }, 1_power, upper_limit, lower_limit, MAX_DEVIATION, min_motor_power, extend_proportinal, deceleration, 20_mm, {0.3_power, 100_mm}, 30ms);

            telemetry::add_line("Tuning finished");
            telemetry::add_data("min motor power", to_string(min_motor_power));
            telemetry::add_data("deceleration", to_string(deceleration));
            telemetry::add_data("correction proportional", to_string(extend_proportinal));
            telemetry::update(); });

        gamepad1.b.add_press_listener([]
                                      {
            constexpr bool tune_min_motor_power = true;
            constexpr bool tune_deceleration = true;
            constexpr bool tune_pd = true;


            Power min_motor_power(0);
            if constexpr (tune_min_motor_power)
                do_min_motor_power_tuning<Millimetre>(outtake::elevator::get_elevation, outtake::elevator::set_power, calc_anti_gravity_power_elevator, 400_mm, 100_mm, min_motor_power);

            constexpr Millimetre upper_limit = 500_mm;
            constexpr Millimetre lower_limit = 100_mm;
            constexpr ::Acceleration added_deceleration_per_loop(0.1);

            ::Acceleration deceleration(5);
            constexpr Millimetre MAX_DEVIATION = 10_mm;
            if constexpr (tune_deceleration)
                do_deceleration_tuning<Metre>(outtake::elevator::get_elevation, outtake::elevator::get_speed, outtake::elevator::set_power, calc_anti_gravity_power_elevator, 1_power, upper_limit, lower_limit, deceleration);

            Divide<Power, Metre>::Type correction_proportional(5.47771);
            if constexpr (tune_pd)
                do_correction_tuning<Metre>(outtake::elevator::get_elevation, outtake::elevator::get_speed, outtake::elevator::set_power, calc_anti_gravity_power_elevator, 1_power, upper_limit, lower_limit, MAX_DEVIATION, min_motor_power, correction_proportional, deceleration, 20_mm, {0.3_power, 100_mm}, 50ms);

            telemetry::add_line("Tuning finished");
            telemetry::add_data("min motor power", to_string(min_motor_power));
            telemetry::add_data("deceleration", to_string(deceleration));
            telemetry::add_data("correction proportional", to_string(correction_proportional));
            telemetry::update(); });

        gamepad1.x.add_press_listener([]
                                      {
            constexpr bool tune_min_motor_power = true;
            constexpr bool tune_deceleration = true;
            constexpr bool tune_pd = true;


            Power min_motor_power(0);
            if constexpr (tune_min_motor_power)
                do_min_motor_power_tuning<Degree>(outtake::arm::get_rotation, outtake::arm::set_power, calc_anti_gravity_power_arm, 120_deg, 20_deg, min_motor_power);

            constexpr Degree upper_limit = 130_deg;
            constexpr Degree lower_limit = 10_deg;

            AngularAcceleration deceleration(0);
            constexpr Degree MAX_DEVIATION(0.5);
            if constexpr (tune_deceleration)
                do_deceleration_tuning<Degree>(outtake::arm::get_rotation, outtake::arm::get_rotation_speed, outtake::arm::set_power, calc_anti_gravity_power_arm, 1_power, upper_limit, lower_limit, deceleration);

            Divide<Power, Degree>::Type correction_proportinal(0);
            if constexpr (tune_pd)
                do_correction_tuning<Degree>(outtake::arm::get_rotation, outtake::arm::get_rotation_speed, outtake::arm::set_power, calc_anti_gravity_power_arm, 1_power, upper_limit, lower_limit, MAX_DEVIATION, min_motor_power, correction_proportinal, deceleration, 5_deg, {0.3_power, 20_deg}, 30ms);

            telemetry::add_line("Tuning finished");
            telemetry::add_data("min motor power", to_string(min_motor_power));
            telemetry::add_data("deceleration", to_string(deceleration));
            telemetry::add_data("correction proportional", to_string(correction_proportinal));
            telemetry::update(); });

        gamepad1.y.add_press_listener([]
                                      {
                                    robot::expansion_hub.get_bulk_data();
                                    intake::extendo::set_power(1_power);
                                    while (intake::extendo::get_extension() < 300_mm)
                                        robot::expansion_hub.get_bulk_data();

                                    intake::extendo::set_power(0_power);
                                    chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
                                    Speed start_speed = intake::extendo::get_extension_speed();
                                    while (intake::extendo::get_extension_speed() != 0_mm / 1_s)
                                        robot::expansion_hub.get_bulk_data();

                                    telemetry::add_data("deceleration", to_string(start_speed / Millisecond(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count())));
                                    telemetry::update(); });

        telemetry::add_line("Initialized");
        telemetry::update();
        linear_op_mode::wait_for_start();
        intake::extendo::lock::unlock();

        while (!linear_op_mode::is_stop_requested())
            ::gamepads::update();

        robot::stop();
    }
}
    */