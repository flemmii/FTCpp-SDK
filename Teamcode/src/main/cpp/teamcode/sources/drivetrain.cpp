//
// Created by FROG on 01.01.2024.
//

#include "../header/drivetrain.h"

using namespace sdk;

namespace drivetrain {
    // Drive_motors:
    template<typename T>
    Drive_motors<T> &Drive_motors<T>::operator=(T value) {
        front_right = value;
        front_left = value;
        rear_right = value;
        rear_left = value;
        return *this;
    }

    template<typename T>
    void Drive_motors<T>::for_each(std::function<void(T &)> func) {
        func(front_right);
        func(front_left);
        func(rear_right);
        func(rear_left);
    }

    template<typename T>
    void
    Drive_motors<T>::for_each(std::function<void(T &, Drive_motor_names)> func) {
        func(front_right, Drive_motor_names::FRONT_RIGHT);
        func(front_left, Drive_motor_names::FRONT_LEFT);
        func(rear_right, Drive_motor_names::REAR_RIGHT);
        func(rear_left, Drive_motor_names::REAR_LEFT);
    }

    template<typename T>
    typename std::enable_if<std::is_floating_point<T>::value, Drive_motors<T>>::type
    fabs(const Drive_motors<T> &obj) {
        return {std::fabs(obj.front_right), std::fabs(obj.front_left),
                std::fabs(obj.rear_right), std::fabs(obj.rear_left)};
    }

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, Drive_motors<T>>::type
    operator+(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2) {
        return {motor1.front_right + motor2.front_right, motor1.front_left + motor2.front_left,
                motor1.rear_right + motor2.rear_right, motor1.rear_left + motor2.rear_left};
    }

    template<typename T>
    template<typename U>
    typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<T> &>::type
    Drive_motors<T>::operator+=(const Drive_motors<U> &other) {
        front_right += other.front_right;
        front_left += other.front_left;
        rear_right += other.rear_right;
        rear_left += other.rear_left;
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, Drive_motors<T>>::type
    operator-(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2) {
        return {motor1.front_right - motor2.front_right, motor1.front_left - motor2.front_left,
                motor1.rear_right - motor2.rear_right, motor1.rear_left - motor2.rear_left};
    }

    template<typename T>
    template<typename U>
    typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<T> &>::type
    Drive_motors<T>::operator/=(const U &value) {
        front_right /= value;
        front_left /= value;
        rear_right /= value;
        rear_left /= value;
        return *this;
    }

    template<typename T>
    template<typename U>
    typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<T> &>::type
    Drive_motors<T>::operator*=(const U &value) {
        front_right *= value;
        front_left *= value;
        rear_right *= value;
        rear_left *= value;
        return *this;
    }

    // ------------------------------------------------------------------------------------------------------------
    // Actual drivetrain
    // -------------------------------------------------------------------------------------------------------------

    bool use_headless = true;
    bool use_gyro_support = false;
    bool use_centripetal_force = true;
    int ms_from_zero_to_max_speed = 0;

    double min_motor_power = 4.5 * WEIGHT_OF_ROBOT / (12 * GEAR_RATIO_DRIVETRAIN);

    bool set_new_manual_target_speed = false;
    Drive_motors<double> manual_set_target_speeds(0);
    Drive_motors<double> target_powers(0);
    Drive_motors<double> last_target_speeds(0);

    PID_controller pid_turn(0.001, 0.0001, 0.01);

    double previous_y_dash = 0;

    chrono::high_resolution_clock::time_point last_time = chrono::high_resolution_clock::now();

    Drive_motors<Dc_motor_ex> drive_motors;

    void init() {
        drive_motors.front_left = hardware_map::get(sdk::DcMotorEx, "front_left");
        drive_motors.front_right = hardware_map::get(sdk::DcMotorEx, "front_right");
        drive_motors.rear_left = hardware_map::get(sdk::DcMotorEx, "rear_left");
        drive_motors.rear_right = hardware_map::get(sdk::DcMotorEx, "rear_right");

        drive_motors.for_each([](Dc_motor_ex &motor) {
            motor.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
        });

        drive_motors.for_each([](Dc_motor_ex &motor) {
            motor.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
        });

        drive_motors.for_each([](Dc_motor_ex &motor) {
            motor.set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
        });

        drive_motors.front_left.set_direction(Dc_motor_ex::Direction::REVERSE);
        drive_motors.rear_left.set_direction(Dc_motor_ex::Direction::REVERSE);

        stop();
    }

    Vec2d calculate_headless(double forwards, double sidewards) {
        double rotation_rad = track_position::get_rotation_rad();
        double sinus = sin(rotation_rad);
        double cosinus = cos(rotation_rad);

        return {-forwards * sinus + sidewards * cosinus, forwards * cosinus + sidewards * sinus};
    }

    Drive_motors<double> calculate_gyro_support(Drive_motors<double> target_speeds) {
        if (target_speeds.front_right + target_speeds.rear_right ==
            target_speeds.front_left + target_speeds.rear_left) {
            double errorTurn = -robot::imu.get_angular_velocity().z;
            double turn = pid_turn.calculate_pid_algorithm(std::fabs(errorTurn)) *
                          ((errorTurn > 0) - (errorTurn < 0));

            return {-turn, turn, -turn, turn};
        }
        return {0, 0, 0, 0};
    }

    Drive_motors<double> calculate_controlled_acceleration(Drive_motors<double> target_speeds) {
        Drive_motors<double> change_of_speeds =
                fabs(target_speeds) - fabs(last_target_speeds);

        double max_change_of_motor_speed = max(
                {change_of_speeds.front_right, change_of_speeds.front_left,
                 change_of_speeds.rear_right, change_of_speeds.rear_left});

        double duration_loop = (double) (duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - last_time).count());
        double max_change_of_speed_allowed;

        if (duration_loop < 100)
            max_change_of_speed_allowed = duration_loop / ms_from_zero_to_max_speed;
        else
            max_change_of_speed_allowed = 20.0 / ms_from_zero_to_max_speed;

        if (max_change_of_motor_speed > max_change_of_speed_allowed) {
            target_speeds.for_each(
                    [change_of_speeds, max_change_of_motor_speed, max_change_of_speed_allowed](
                            double &target_speed,
                            Drive_motors<double>::Drive_motor_names name) {

                        if (change_of_speeds[name] > 0 && target_speed != 0) {
                            target_speed = last_target_speeds[name] + (change_of_speeds[name] /
                                                                       (max_change_of_motor_speed /
                                                                        max_change_of_speed_allowed)) *
                                                                      ((target_speed > 0) -
                                                                       (target_speed < 0));
                        }

                    });

            auto abs_target_speeds = fabs(target_speeds);
            double max = std::max({abs_target_speeds.front_right, abs_target_speeds.front_left,
                                   abs_target_speeds.rear_right, abs_target_speeds.rear_left});
            if (max < min_motor_power)
                target_speeds *= min_motor_power / max;
        }

        last_target_speeds = target_speeds;

        last_time = chrono::high_resolution_clock::now();
        return target_speeds;
    }

    Drive_motors<double> calculate_centripetal_force() {
        // TODO: Someone smarter needs to do this
        Position current_power = get_target_power();

        if (current_power.y != 0 && current_power.x != 0) {
            double y_dash = current_power.y / current_power.x;
            double y_dash_dash = (y_dash - previous_y_dash) / current_power.x;

            // radius positive = left turn; negative = right turn
            double radius = (1 + y_dash * y_dash) * sqrt(1 + y_dash * y_dash) / y_dash_dash;

            double total_centripetal_force = WEIGHT_OF_ROBOT * (current_power.x * current_power.x +
                                                                current_power.y * current_power.y) /
                                             radius;
            double rotation_of_centripetal_force = 0;
            if (current_power.y != 0) {
                rotation_of_centripetal_force =
                        atan(current_power.x / current_power.y) - 2 / M_PI;
            } else if (current_power.x < 0)
                rotation_of_centripetal_force = M_PI;

            Vec2d centripetal_force(
                    total_centripetal_force * sin(rotation_of_centripetal_force),
                    total_centripetal_force * cos(rotation_of_centripetal_force));

            previous_y_dash = y_dash;

            Vec2d headless_power = calculate_headless(centripetal_force.y, centripetal_force.x);

            return {headless_power.y - headless_power.x, headless_power.y + headless_power.x,
                    headless_power.y + headless_power.x, headless_power.y - headless_power.x};
        } else {
            previous_y_dash = 0;
            return {0, 0, 0, 0};
        }
    }

    void do_drive() {
        if (!set_new_manual_target_speed && !use_centripetal_force && !use_gyro_support &&
            (ms_from_zero_to_max_speed <= 0 || last_target_speeds == target_powers))
            return;

        // TODO: adjust this constant
        min_motor_power = 5 * WEIGHT_OF_ROBOT / (robot::control_hub.get_input_voltage(
                voltage_unit::voltage_unit_type::VOLTS) * GEAR_RATIO_DRIVETRAIN);

        Drive_motors<double> target_powers = manual_set_target_speeds;

        if (use_centripetal_force)
            target_powers += calculate_centripetal_force();

        if (use_gyro_support)
            target_powers += calculate_gyro_support(target_powers);

        auto abs_target_speeds = fabs(target_powers);
        double max = std::max({abs_target_speeds.front_right, abs_target_speeds.front_left,
                               abs_target_speeds.rear_right, abs_target_speeds.rear_left});

        if (max > 1.0)
            target_powers /= max;

        if (ms_from_zero_to_max_speed > 0)
            target_powers = calculate_controlled_acceleration(target_powers);

        target_powers.for_each([](double &target_speed) {
            if (std::fabs(target_speed) < min_motor_power)
                target_speed = 0;
        });

        drive_motors.for_each(
                [&target_powers](Dc_motor_ex &drive_motor,
                                 Drive_motors<Dc_motor_ex>::Drive_motor_names name) {
                    drive_motor.set_power(
                            target_powers[static_cast<Drive_motors<double>::Drive_motor_names>(name)]);
                });

        drivetrain::target_powers = target_powers;
        set_new_manual_target_speed = false;
    }

    void drive(double forwards, double sidewards, double turn, double speed_multiplier) {
        if (use_headless) {
            Vec2d power_headless = calculate_headless(forwards, sidewards);
            forwards = power_headless.y;
            sidewards = power_headless.x;
        }

        sidewards *= sqrt(2);

        Drive_motors<double> target_speeds(forwards - sidewards - turn,
                                           forwards + sidewards + turn,
                                           forwards + sidewards - turn,
                                           forwards - sidewards + turn);

        auto abs_target_speeds = fabs(target_speeds);
        double max = std::max({abs_target_speeds.front_right, abs_target_speeds.front_left,
                               abs_target_speeds.rear_right, abs_target_speeds.rear_left});

        if (max > 1.0)
            target_speeds /= max;

        target_speeds *= speed_multiplier;

        set_new_manual_target_speed = true;
        manual_set_target_speeds = target_speeds;
    }

    void stop() {
        drive_motors.for_each([](Dc_motor_ex &motor) {
            motor.set_power(0);
        });
    }

    Position get_target_power() {
        double forwards =
                (target_powers.front_right + target_powers.front_left + target_powers.rear_right +
                 target_powers.rear_left) / 4;
        double sidewards =
                (-target_powers.front_right + target_powers.front_left + target_powers.rear_right -
                 target_powers.rear_left) / 4;
        double turn =
                (-target_powers.front_right + target_powers.front_left - target_powers.rear_right +
                 target_powers.rear_left) / 4;

        double rotation_rad = track_position::get_rotation_rad();
        double sinus = sin(rotation_rad);
        double cosinus = cos(rotation_rad);

        double x = sidewards * cosinus + forwards * sinus;
        double y = forwards * cosinus - sidewards * sinus;

        return {x, y, turn};
    }

    bool should_drive() {
        return !(target_powers == 0);
    }

    double get_min_motor_power() {
        return min_motor_power;
    }

    void preset_for_auto() {
        use_headless = true;
        use_gyro_support = false;
        use_centripetal_force = false;
        ms_from_zero_to_max_speed = 100;
    }

    void preset_for_tele_op() {
        use_headless = true;
        use_gyro_support = false;
        use_centripetal_force = true;
        ms_from_zero_to_max_speed = 100;
    }
}