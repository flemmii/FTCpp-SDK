#pragma once

#include "sdk.hpp"
#include "Units-master/Units.h"
#include "utils/Position.hpp"
#include "robot.hpp"
#include "constants.hpp"

#include <functional>

namespace drivetrain
{
    constexpr Newton FRICTION = 22_N;                         // Measured
    constexpr double STATIC_FRICTION_COEFFICIENT = 11.0 / 30; // For GoBilda Wheels on the field tiles
    constexpr Newton STATIC_FRICTION = robot::MASS * GRAVITY * STATIC_FRICTION_COEFFICIENT;

    constexpr double GEAR_RATIO = 11.25; // = 13.7
    constexpr Millimetre TRACK_WIDTH = 317_mm;
    constexpr Millimetre WHEEL_DIAMETER = 96_mm;
    // GoBilda Wheels: 96 mm
    // Tetrix Wheels: 98 mm
    // Old Frox Wheels: 100 mm

    constexpr bool ENCODER_PLUGGED_IN = true;
    constexpr Divide<Millimetre, Tick>::Type MM_PER_TICK = (WHEEL_DIAMETER * M_PI / 1_rev) / (GEAR_RATIO * motors::TICKS_PER_REVOLUTION);

    /**
     * @brief A struct to represent the motors of the drivetrain.
     *
     * This struct is used to represent the four motors of the drivetrain: front right, front left, rear right, and rear left.
     * It provides various functions to manipulate and access the motors.
     *
     * The actual implementation of this struct can be found in the `drivetrain.cpp` file.
     */
    template <typename T>
    struct Drive_motors
    {
        T front_right;
        T front_left;
        T rear_right;
        T rear_left;

        enum class Drive_motor_names
        {
            FRONT_RIGHT,
            FRONT_LEFT,
            REAR_RIGHT,
            REAR_LEFT
        };

        T &operator[](Drive_motor_names name)
        {
            switch (name)
            {
            case Drive_motor_names::FRONT_RIGHT:
                return front_right;
            case Drive_motor_names::FRONT_LEFT:
                return front_left;
            case Drive_motor_names::REAR_RIGHT:
                return rear_right;
            case Drive_motor_names::REAR_LEFT:
                return rear_left;
            default:
                throw std::out_of_range("Index out of range");
            }
        }

        const T &operator[](Drive_motor_names name) const
        {
            switch (name)
            {
            case Drive_motor_names::FRONT_RIGHT:
                return front_right;
            case Drive_motor_names::FRONT_LEFT:
                return front_left;
            case Drive_motor_names::REAR_RIGHT:
                return rear_right;
            case Drive_motor_names::REAR_LEFT:
                return rear_left;
            default:
                throw std::out_of_range("Index out of range");
            }
        }

        Drive_motors() = default;

        Drive_motors(T front_right, T front_left, T rear_right, T rear_left)
            : front_right(front_right), front_left(front_left), rear_right(rear_right),
              rear_left(rear_left) {}

        explicit Drive_motors(T all)
            : front_right(all), front_left(all), rear_right(all), rear_left(all) {}

        Drive_motors<T> &operator=(T value);

        void for_each(std::function<void(T &)> func);

        void for_each(std::function<void(T &, Drive_motor_names)> func);

        template <typename U>
        Drive_motors<T> &abs(const Drive_motors<T> &obj);

        template <typename U>
        friend Drive_motors<T> &fabs(const Drive_motors<T> &motor);

        template <typename U>
        friend Drive_motors<T> operator+(const Drive_motors<U> &motor1, const Drive_motors<U> &motor2);

        template <typename U>
        Drive_motors<T> &operator+=(const Drive_motors<U> &other);

        template <typename U>
        friend Drive_motors<T> operator-(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2);

        Drive_motors<T> &operator/=(const double &value);

        template <typename U>
        friend Drive_motors<U> operator*(const Drive_motors<U> &motor, const double &value);

        friend constexpr Drive_motors operator/(const Drive_motors &motor, const double &value)
        {
            return {motor.front_right / value, motor.front_left / value,
                    motor.rear_right / value, motor.rear_left / value};
        }

        template <typename U>
        Drive_motors<T> &operator*=(const U &value);

        // Comparison operators need to be in the header
        friend bool operator==(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2)
        {
            return motor1.front_right == motor2.front_right &&
                   motor1.front_left == motor2.front_left &&
                   motor1.rear_right == motor2.rear_right &&
                   motor1.rear_left == motor2.rear_left;
        }

        template <typename U>
        friend bool operator==(const Drive_motors<T> &motor1, const U &value)
        {
            return motor1.front_right == value &&
                   motor1.front_left == value &&
                   motor1.rear_right == value &&
                   motor1.rear_left == value;
        }

        friend bool operator<=(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2)
        {
            return motor1.front_right <= motor2.front_right &&
                   motor1.front_left <= motor2.front_left &&
                   motor1.rear_right <= motor2.rear_right &&
                   motor1.rear_left <= motor2.rear_left;
        }

        friend bool operator<(const Drive_motors<T> &motor1, const T &value)
        {
            return motor1.front_right < value &&
                   motor1.front_left < value &&
                   motor1.rear_right < value &&
                   motor1.rear_left < value;
        }

        template <typename S>
        friend S &operator<<(S &os, const Drive_motors &motors)
        {
            os << "front right: " << motors.front_right << ", front left: " << motors.front_left
               << ", rear right: " << motors.rear_right << ", rear left: " << motors.rear_left;
            return os;
        }
    };

    extern Power min_motor_power;

    extern bool clip_lower_powers;

    /**
     * @brief A boolean to represent if the robot should use gyro support.
     *
     * If this boolean is true, the robot will use gyro support.
     * With gyro support, the robot can measure its orientation to control its movement more accurately.
     */
    extern bool use_gyro_support;

    /**
     * @brief A boolean to represent if the robot should use centripetal force.
     *
     * If this boolean is true, the robot will use centripetal force.
     * With centripetal force, the robot can maintain a constant speed when turning.
     */
    extern bool use_centripetal_force;

    extern bool use_controlled_acceleration;

    extern Drive_motors<Power> target_powers;

    /**
     * @brief Initializes the drivetrain of the robot.
     *
     * This function is responsible for initializing the drivetrain of the robot.
     * It sets up the necessary hardware and software components for the drivetrain to function properly.
     *
     * This function should be called before the main control loop of the robot starts.
     */
    void init();

    Power update_min_motor_power();

    template <typename Unit>
    Position<false, Unit, Unit> conv_to_pos(const Drive_motors<Unit> &motor_powers);

    template <typename Unit>
    Drive_motors<Unit> conv_to_motor_powers(const Position<false, Unit, Unit> &powers);

    Drive_motors<Speed> get_motor_speeds();

    Drive_motors<Power> conv_to_motor_powers(const Drive_motors<Newton> &forces, const Drive_motors<Speed> &speeds);

    Drive_motors<Power> apply_force_to_robot(const Drive_motors<Newton> &force, const Drive_motors<Speed> &speeds);

    template <bool field_centric>
    Drive_motors<Power> apply_force_to_robot(const Position<field_centric, Newton, Newton> &force, const Drive_motors<Speed> &speeds);

    /**
     * @brief Drives the robot.
     *
     * This function is responsible for driving the robot. It takes in parameters for forward motion,
     * sideways motion, turning, and a speed multiplier.
     *
     * @param forwards The amount of forward motion. Positive values move the robot forward,
     * negative values move it backward.
     * @param sidewards The amount of sideways motion. Positive values move the robot to the right,
     * negative values move it to the left.
     * @param turn The amount of turning. Positive values turn the robot to the right,
     * negative values turn it to the left.
     * @param speed_multiplier A multiplier for the speed of the robot. Defaults to 1 if not provided.
     */
    template <bool field_centric>
    void drive(const Position<field_centric, Power, Power> &powers, const double &speed_multiplier = 1);

    void drive(Drive_motors<Power> target_powers, const double &speed_multiplier = 1);

    /**
     * @brief Retrieves the target power for the drivetrain.
     *
     * This function is responsible for retrieving the target power for the drivetrain of the robot.
     * The target power is calculated based on various factors such as manual input, gyro support, and centripetal force.
     *
     * This function should be called when the current target power of the drivetrain needs to be known.
     *
     * @return Position The current target power of the drivetrain.
     */
    template <bool field_centric>
    Position<field_centric, Power, Power> get_target_power();

    /**
     * @brief Sets the zero power behavior for the drivetrain motors.
     *
     * This function is responsible for setting the zero power behavior for the drivetrain motors.
     * The zero power behavior determines what the motors should do when their power is set to zero.
     * There are two possible behaviors:
     * - sdk::Dc_motor_ex::Zero_power_behavior::BRAKE: The motors will try to stop as quickly as possible by actively braking.
     * - sdk::Dc_motor_ex::Zero_power_behavior::FLOAT: The motors will coast to a stop without actively braking.
     *
     * @param behavior The desired zero power behavior. This should be either sdk::Dc_motor_ex::Zero_power_behavior::BRAKE or sdk::Dc_motor_ex::Zero_power_behavior::FLOAT.
     */
    void set_zero_power_behavior(const sdk::Dc_motor_ex::Zero_power_behavior &behavior);

    /**
     * @brief Sets the preset for auto mode.
     *
     * This function is responsible for setting the preset for auto mode.
     * In auto mode, the robot uses float as zero power behavior,
     * does not use gyro support, and does not use centripetal force.
     * The time from zero to max speed is set to 100 milliseconds.
     */
    void preset_for_auto();

    /**
     * @brief Sets the preset for tele operation mode.
     *
     * This function is responsible for setting the preset for tele operation mode.
     * In tele operation mode, the robot uses brake as zero power behavior,
     * does not use gyro support, and uses centripetal force.
     * The time from zero to max speed is set to 100 milliseconds.
     */
    void preset_for_tele_op();
}
