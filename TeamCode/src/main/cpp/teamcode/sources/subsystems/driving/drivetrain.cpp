#include "subsystems/driving/drivetrain.hpp"

#include "robot.hpp"
#include "utils/PD_controller.hpp"
#include "Units-master/Maths.h"
#include "subsystems/subsystem_utils.hpp"
#include "utils/Location.hpp"
#include "subsystems/driving/pinpoint.hpp"

using namespace sdk;

namespace drivetrain
{
    // Drive_motors:
    template <typename T>
    Drive_motors<T> &Drive_motors<T>::operator=(T value)
    {
        front_right = value;
        front_left = value;
        rear_right = value;
        rear_left = value;
        return *this;
    }

    template <typename T>
    void Drive_motors<T>::for_each(std::function<void(T &)> func)
    {
        func(front_right);
        func(front_left);
        func(rear_right);
        func(rear_left);
    }

    template <typename T>
    void
    Drive_motors<T>::for_each(std::function<void(T &, Drive_motor_names)> func)
    {
        func(front_right, Drive_motor_names::FRONT_RIGHT);
        func(front_left, Drive_motor_names::FRONT_LEFT);
        func(rear_right, Drive_motor_names::REAR_RIGHT);
        func(rear_left, Drive_motor_names::REAR_LEFT);
    }

    template <typename T>
    Drive_motors<T> abs(const Drive_motors<T> &obj)
    {
        return {abs(obj.front_right), abs(obj.front_left),
                abs(obj.rear_right), abs(obj.rear_left)};
    }

    template <typename T>
    Drive_motors<T> fabs(const Drive_motors<T> &obj)
    {
        return {std::fabs(obj.front_right), std::fabs(obj.front_left),
                std::fabs(obj.rear_right), std::fabs(obj.rear_left)};
    }

    template <typename T>
    Drive_motors<T> operator+(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2)
    {
        return {motor1.front_right + motor2.front_right, motor1.front_left + motor2.front_left,
                motor1.rear_right + motor2.rear_right, motor1.rear_left + motor2.rear_left};
    }

    template <typename T>
    template <typename U>
    Drive_motors<T> &Drive_motors<T>::operator+=(const Drive_motors<U> &other)
    {
        front_right += other.front_right;
        front_left += other.front_left;
        rear_right += other.rear_right;
        rear_left += other.rear_left;
        return *this;
    }

    template <typename T>
    Drive_motors<T> operator-(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2)
    {
        return {motor1.front_right - motor2.front_right, motor1.front_left - motor2.front_left,
                motor1.rear_right - motor2.rear_right, motor1.rear_left - motor2.rear_left};
    }

    template <typename T>
    Drive_motors<T> &Drive_motors<T>::operator/=(const double &value)
    {
        front_right /= value;
        front_left /= value;
        rear_right /= value;
        rear_left /= value;
        return *this;
    }

    template <typename T>
    Drive_motors<T> operator*(const Drive_motors<T> &motor, const double &value)
    {
        return {motor.front_right * value, motor.front_left * value,
                motor.rear_right * value, motor.rear_left * value};
    }

    template <typename T>
    template <typename U>
    Drive_motors<T> &Drive_motors<T>::operator*=(const U &value)
    {
        front_right *= value;
        front_left *= value;
        rear_right *= value;
        rear_left *= value;
        return *this;
    }

    // ------------------------------------------------------------------------------------------------------------
    // Actual drivetrain
    // -------------------------------------------------------------------------------------------------------------

    bool clip_lower_powers = true;
    bool use_gyro_support = false;
    bool use_centripetal_force = true;
    bool use_controlled_acceleration = true;

    Power min_motor_power = 0_power;

    bool driving = false;
    Drive_motors<Power> manual_set_target_powers(0_power);
    Drive_motors<Power> target_powers(0_power);

    Drive_motors<Dc_motor_ex> drive_motors;

    void init()
    {
        update_min_motor_power();
        drive_motors.front_left = hardware_map::get<Dc_motor_ex>("drivetrain_front_left");
        drive_motors.front_right = hardware_map::get<Dc_motor_ex>("drivetrain_front_right");
        drive_motors.rear_left = hardware_map::get<Dc_motor_ex>("drivetrain_rear_left");
        drive_motors.rear_right = hardware_map::get<Dc_motor_ex>("drivetrain_rear_right");

        drive_motors.for_each([](Dc_motor_ex &motor)
                              { motor.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER); });

        drive_motors.for_each([](Dc_motor_ex &motor)
                              { motor.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER); });

        drive_motors.for_each([](Dc_motor_ex &motor)
                              { motor.set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE); });

        drive_motors.front_left.set_direction(Dc_motor_ex::Direction::REVERSE);
        drive_motors.rear_left.set_direction(Dc_motor_ex::Direction::REVERSE);
    }

    Power update_min_motor_power()
    {
        min_motor_power = subsystem_utils::conv_torque_to_power(FRICTION * WHEEL_DIAMETER / 2, 0_deg / 1_s, GEAR_RATIO) / 2;
        return min_motor_power;
    }

    template <typename Unit>
    Vec2d<false, Unit> conv_to_robot_centric(const Vec2d<true, Unit> &powers)
    {
        const Radian rotation = pinpoint::get_position().rotation;
        const double sinus = maths::Sin(rotation);
        const double cosinus = maths::Cos(rotation);

        return {powers.y * cosinus + powers.x * sinus, -powers.y * sinus + powers.x * cosinus};
    }

    template <typename Unit>
    Vec2d<true, Unit> conv_to_field_centric(const Vec2d<false, Unit> &powers)
    {
        const Radian rotation = pinpoint::get_position().rotation;
        const double sinus = maths::Sin(rotation);
        const double cosinus = maths::Cos(rotation);

        return {powers.sidewards * cosinus + powers.forwards * sinus,
                powers.forwards * cosinus - powers.sidewards * sinus};
    }

    template <typename Unit>
    Position<false, Unit, Unit>
    conv_to_pos(const Drive_motors<Unit> &motor_powers)
    {
        return {(motor_powers.front_right + motor_powers.front_left + motor_powers.rear_right +
                 motor_powers.rear_left) /
                    4,
                (-motor_powers.front_right + motor_powers.front_left + motor_powers.rear_right -
                 motor_powers.rear_left) /
                    4,
                (-motor_powers.front_right + motor_powers.front_left - motor_powers.rear_right +
                 motor_powers.rear_left) /
                    4};
    }

    template <typename Unit>
    Drive_motors<Unit> conv_to_motor_powers(const Position<false, Unit, Unit> &powers)
    {
        return {powers.forwards - powers.sidewards - powers.rotation,
                powers.forwards + powers.sidewards + powers.rotation,
                powers.forwards + powers.sidewards - powers.rotation,
                powers.forwards - powers.sidewards + powers.rotation};
    }

    Drive_motors<Speed> get_motor_speeds()
    {
        if constexpr (ENCODER_PLUGGED_IN)
        {
            Drive_motors<Speed> motor_speeds;
            drive_motors.for_each([&motor_speeds](Dc_motor_ex &motor, Drive_motors<Dc_motor_ex>::Drive_motor_names name)
                                  { motor_speeds[static_cast<Drive_motors<Speed>::Drive_motor_names>(name)] = motor.get_velocity() * (MM_PER_TICK * 1_tick) / 1_s; });
            return motor_speeds;
        }
        else
        {
            const Position<true, Speed, AngularSpeed> speed = pinpoint::get_speed();
            return conv_to_motor_powers<Speed>({conv_to_robot_centric(speed.to_vec()),
                                                pinpoint::conv_to_distance<Second>(speed.rotation) * sqrt(2)});
        }
    }

    Drive_motors<Power> conv_to_motor_powers(const Drive_motors<Newton> &forces, const Drive_motors<Speed> &speeds)
    {
        Drive_motors<Power>
            motor_powers(0_power);
        motor_powers.for_each([&speeds, &forces](Power &motor_power,
                                                 Drive_motors<Power>::Drive_motor_names name)
                              {
                                Drive_motors<Newton>::Drive_motor_names motor_name = static_cast<Drive_motors<Newton>::Drive_motor_names>(name);
                                  if (forces[motor_name] != 0_N)
                                      motor_power = subsystem_utils::conv_torque_to_power(forces[motor_name] * WHEEL_DIAMETER / 2, (speeds[static_cast<Drive_motors<Speed>::Drive_motor_names>(name)] / ((WHEEL_DIAMETER * M_PI) / 1_rev)), GEAR_RATIO); });

        return motor_powers;
    }

    Drive_motors<Power> apply_force_to_robot(const Drive_motors<Newton> &force, const Drive_motors<Speed> &speeds)
    {
        return conv_to_motor_powers(force * 0.25, speeds); // * 0.25, so the whole drivetrain got the force and not every wheel
    }

    template <>
    Drive_motors<Power> apply_force_to_robot<false>(const Position<false, Newton, Newton> &force, const Drive_motors<Speed> &speeds)
    {
        return apply_force_to_robot(conv_to_motor_powers(force), speeds);
    }

    template <>
    Drive_motors<Power> apply_force_to_robot<true>(const Position<true, Newton, Newton> &force, const Drive_motors<Speed> &speeds)
    {
        return apply_force_to_robot<false>({conv_to_robot_centric(force.to_vec()), force.rotation}, speeds);
    }

    Drive_motors<Power> calc_gyro_support(const Drive_motors<Power> &powers)
    {
        static bool already_reset = true;
        static Degree target(0);
        // if the error is 30 degrees or more the robot should respond to it with maximum turn power ( 1 or -1)
        constexpr Degree MAX_EXPECTED_ERROR(45);
        static Degree max_observed_error = 0_deg;

        Position powers_pos = conv_to_pos(powers);
        if ((powers_pos.forwards != 0_power || powers_pos.sidewards != 0_power) && powers_pos.rotation == 0_power)
        {
            already_reset = false;

            Degree error = target - pinpoint::get_position().rotation;
            double scaled_error = error / MAX_EXPECTED_ERROR;
            double turn = 0.6 * pow(scaled_error, 3) + 0.225 * scaled_error * std::fabs(scaled_error) + 0.175 * scaled_error;

            if (abs(error) > abs(max_observed_error))
                max_observed_error = error;

            log(ANDROID_LOG_DEBUG, "DEBUG") << "Rotation error (from gyro support): " << error;
            log(ANDROID_LOG_DEBUG, "DEBUG") << "Max rotation error (from gyro support): " << max_observed_error;
            log(ANDROID_LOG_DEBUG, "DEBUG") << "Turn (from gyro support): " << turn;
            return conv_to_motor_powers<Power>({0_power, 0_power, 0_power});
        }
        else
        {
            already_reset = true;
            target = pinpoint::get_position().rotation;
            // log(ANDROID_LOG_DEBUG, "DEBUG") << "Target rotation (from gyro support): " << target;
            return Drive_motors<Power>(0_power);
        }
    }

    bool apply_controlled_acceleration(Drive_motors<Power> &powers)
    {
        const Power max_power = max({abs(powers.front_right), abs(powers.front_left),
                                     abs(powers.rear_right), abs(powers.rear_left)});
        if (max_power == 0_power)
            return true;

        constexpr Newton force = (STATIC_FRICTION + FRICTION) * 4;

        const Drive_motors<Power> new_powers = apply_force_to_robot({force * powers.front_right / max_power,
                                                                     force * powers.front_left / max_power,
                                                                     force * powers.rear_right / max_power,
                                                                     force * powers.rear_left / max_power},
                                                                    get_motor_speeds());

        bool finished = true;
        powers.for_each([&new_powers, &finished](Power &power, Drive_motors<Power>::Drive_motor_names name)
                        {
                    const Power new_power = new_powers[static_cast<Drive_motors<Power>::Drive_motor_names>(name)];
                    if (abs(new_power) < abs(power))
                    {
                        power = new_power;
                        finished = false;
                    } });

        return finished;
    }

    Drive_motors<Power> calc_centripetal_force(const Drive_motors<Power> &powers)
    {
        static Radian last_angle(0);
        static Vec2d<true, Millimetre> last_position(0_mm, 0_mm);

        Drive_motors<Power> centripetal_force_motor_powers(0_power);

        const Position current_power_pos = conv_to_pos(powers);
        const Vec2d current_power = conv_to_field_centric(current_power_pos.to_vec());

        const Radian current_angle = maths::Arctan(current_power.x, current_power.y);
        Radian angle_difference = current_angle - last_angle;
        if (angle_difference > 180_deg)
            angle_difference -= 360_deg;
        else if (angle_difference < -180_deg)
            angle_difference += 360_deg;

        const Vec2d speed = pinpoint::get_speed().to_vec();
        const Vec2d current_position = pinpoint::get_position().to_vec();

        const Millimetre distance = (current_position - last_position).total_distance();

        if (angle_difference != 0_rad && distance != 0_mm)
        {
            const Millimetre radius = sqrt(distance * distance / (2 * (1 - maths::Cos(angle_difference))));
            const Newton centripetal_force = robot::MASS * (speed.x * speed.x + speed.y * speed.y) / radius;

            const Radian rotation_of_centripetal_force = current_angle + 90_deg * maths::Sgn(angle_difference);
            centripetal_force_motor_powers = apply_force_to_robot<true>({centripetal_force * maths::Sin(rotation_of_centripetal_force), centripetal_force * maths::Cos(rotation_of_centripetal_force), 0_N}, {0_m / 1_s, 0_m / 1_s, 0_m / 1_s, 0_m / 1_s});
        }
        last_position = current_position;
        last_angle = current_angle;

        return centripetal_force_motor_powers;
    }

    bool do_drive()
    {
        Drive_motors<Power> manual_set_target_powers = drivetrain::manual_set_target_powers;
        Drive_motors<Power> target_powers = manual_set_target_powers;
        if (use_gyro_support)
            target_powers += calc_gyro_support(target_powers);

        const Power min_motor_power = update_min_motor_power();

        bool finished_controlled_acceleration = true;
        if (!(manual_set_target_powers == 0_power || (abs(manual_set_target_powers) < min_motor_power && clip_lower_powers)))
        {
            if (use_centripetal_force)
                target_powers += calc_centripetal_force(target_powers);

            Position target_powers_pos = conv_to_pos(target_powers);
            target_powers_pos.sidewards *= sqrt(2);
            target_powers = conv_to_motor_powers(target_powers_pos);

            const Drive_motors<Power> abs_target_powers = abs(target_powers);
            const double max = std::max({abs_target_powers.front_right, abs_target_powers.front_left,
                                         abs_target_powers.rear_right, abs_target_powers.rear_left})
                                   .Value();

            if (max > 1)
                target_powers /= max;

            if (clip_lower_powers)
            {
                target_powers.for_each([&min_motor_power](Power &target_power)
                                       {
                                        if (abs(target_power) < min_motor_power)
                                            target_power = 0_power; });
            }

            if (use_controlled_acceleration && !(target_powers == 0_power))
                finished_controlled_acceleration = apply_controlled_acceleration(target_powers);
        }

        drivetrain::target_powers = target_powers;

        drive_motors.for_each([&target_powers](Dc_motor_ex &drive_motor,
                                               Drive_motors<Dc_motor_ex>::Drive_motor_names name)
                              { drive_motor.set_power(
                                    target_powers[static_cast<Drive_motors<Power>::Drive_motor_names>(name)].Value()); });

        return finished_controlled_acceleration;
    }

    void start_driving()
    {
        if (driving || (!use_gyro_support && do_drive()))
            return;

        driving = true;

        task_scheduler::register_task((function<bool()>)[] { return !use_gyro_support && do_drive(); }, 50ms, false, []
                                      { driving = false; });
    }

    void drive(Drive_motors<Power> target_powers, const double &speed_multiplier)
    {
        log(ANDROID_LOG_DEBUG, "DEBUG") << "target powers 1: " << target_powers.front_right;

        const Drive_motors<Power> abs_target_powers = abs(target_powers);
        const double max = std::max({abs_target_powers.front_right, abs_target_powers.front_left,
                                     abs_target_powers.rear_right, abs_target_powers.rear_left})
                               .Value();

        if (max > 1)
            target_powers /= max;

        target_powers *= speed_multiplier;

        manual_set_target_powers = target_powers;
        start_driving();
    }

    template <>
    void drive(const Position<false, Power, Power> &powers, const double &speed_multiplier)
    {
        drive(conv_to_motor_powers(powers));
    }

    template <>
    void drive(const Position<true, Power, Power> &powers, const double &speed_multiplier)
    {
        drive<false>({conv_to_robot_centric(powers.to_vec()), powers.rotation}, speed_multiplier);
    }

    template <>
    Position<false, Power, Power> get_target_power()
    {
        return conv_to_pos(target_powers);
    }

    template <>
    Position<true, Power, Power> get_target_power()
    {
        const Position position = get_target_power<false>();
        return {conv_to_field_centric(position.to_vec()), position.rotation};
    }

    void set_zero_power_behavior(const Dc_motor_ex::Zero_power_behavior &behavior)
    {
        drive_motors.for_each([behavior](Dc_motor_ex &motor)
                              { motor.set_zero_power_behavior(behavior); });
    }

    void preset_for_auto()
    {
        set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::FLOAT);
        clip_lower_powers = true;
        use_gyro_support = false;
        use_centripetal_force = false;
        use_controlled_acceleration = true;
    }

    void preset_for_tele_op()
    {
        set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
        clip_lower_powers = false;
        use_gyro_support = true;
        use_centripetal_force = true;
        use_controlled_acceleration = false;
    }
}