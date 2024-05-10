//
// Created by FROG on 01.01.2024.
//

#ifndef FTCROBOTCONTROLLER_DRIVETRAIN_H
#define FTCROBOTCONTROLLER_DRIVETRAIN_H

#include "sdk/hardware/Dc_motor_ex.h"
#include "sdk/hardware/hardware_map.h"
#include "sdk/navigation/Voltage_unit.h"

#include "robot.h"
#include "Position.h"
#include "PID_controller.h"

namespace drivetrain {
    /**
     * @brief A struct to represent the motors of the drivetrain.
     *
     * This struct is used to represent the four motors of the drivetrain: front right, front left, rear right, and rear left.
     * It provides various functions to manipulate and access the motors.
     *
     * The actual implementation of this struct can be found in the `drivetrain.cpp` file.
     */
    template<typename T>
    struct Drive_motors {
        T front_right;
        T front_left;
        T rear_right;
        T rear_left;

        enum class Drive_motor_names {
            FRONT_RIGHT,
            FRONT_LEFT,
            REAR_RIGHT,
            REAR_LEFT
        };

        T &operator[](Drive_motor_names name) {
            switch (name) {
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

        const T &operator[](Drive_motor_names name) const {
            switch (name) {
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

        template<typename U>
        friend typename std::enable_if<std::is_floating_point<U>::value, Drive_motors<U>>::type
        fabs(const Drive_motors<T> &motor);

        template<typename U>
        friend typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<U>>::type
        operator+(const Drive_motors<U> &motor1, const Drive_motors<U> &motor2);

        template<typename U>
        typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<T> &>::type
        operator+=(const Drive_motors<U> &other);

        template<typename U>
        friend typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<U>>::type
        operator-(const Drive_motors<T> &obj1, const Drive_motors<T> &obj2);

        template<typename U>
        typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<T> &>::type
        operator/=(const U &value);

        template<typename U>
        typename std::enable_if<std::is_arithmetic<U>::value, Drive_motors<T> &>::type
        operator*=(const U &value);

        // Comparison operators need to be in the header
        friend bool operator==(const Drive_motors<T> &motor1, const Drive_motors<T> &motor2) {
            return motor1.front_right == motor2.front_right &&
                   motor1.front_left == motor2.front_left &&
                   motor1.rear_right == motor2.rear_right &&
                   motor1.rear_left == motor2.rear_left;
        }

        template<typename U>
        friend typename std::enable_if<std::is_arithmetic<U>::value, bool>::type
        operator==(const Drive_motors<T> &motor1, const U &value) {
            return motor1.front_right == value &&
                   motor1.front_left == value &&
                   motor1.rear_right == value &&
                   motor1.rear_left == value;
        }
    };

    /**
     * @brief A boolean to represent if the robot should use headless mode.
     *
     * If this boolean is true, the robot will use headless mode.
     * In headless mode, the robot's direction of movement is relative to its initial orientation, not its current orientation.
     */
    extern bool use_headless;

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

    /**
     * @brief An integer to represent the time from zero to max speed.
     *
     * This integer represents the time in milliseconds it takes for the robot to accelerate from zero to its maximum speed.
     * A value of 0 means there is no controlled acceleration.
     */
    extern int ms_from_zero_to_max_speed;

    /**
     * @brief Initializes the drivetrain of the robot.
     *
     * This function is responsible for initializing the drivetrain of the robot.
     * It sets up the necessary hardware and software components for the drivetrain to function properly.
     *
     * This function should be called before the main control loop of the robot starts.
     */
    void init();

    /**
     * @brief Controls the drivetrain of the robot.
     *
     * This function is responsible for controlling the drivetrain of the robot.
     * It calculates the target speeds for the motors based on various factors such as:
     * - Manual input
     * - Gyro support
     * - Centripetal force
     *
     * After calculating the target speeds, it sets the power of the motors to these target speeds.
     *
     * This function should be called in the main control loop of the robot.
     * It continuously updates the power of the motors based on the current control inputs and sensor readings.
     */
    void do_drive();

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
    void drive(double forwards, double sidewards, double turn, double speed_multiplier = 1);

    /**
     * @brief Stops the drivetrain of the robot.
     *
     * This function is responsible for stopping the drivetrain of the robot.
     * It sets the power of all motors to zero, effectively stopping the robot's movement.
     *
     * This function should be called when the robot needs to stop moving immediately.
     */
    void stop();

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
    Position get_target_power();

    /**
     * @brief Checks if the robot should drive.
     *
     * This function is responsible for determining if the robot should drive.
     * It checks if the target speeds for all motors are not zero, which means the robot has a target speed and should be driving.
     *
     * @return bool Returns true if the robot should drive, false otherwise.
     */
    bool should_drive();

    /**
     * @brief Retrieves the minimum motor power.
     *
     * This function is responsible for retrieving the minimum motor power.
     * The minimum motor power is the lowest power that can be applied to the motors for them to start moving.
     * The actual implementation of this function can be found in the `drivetrain.cpp` file.
     *
     * @return double The minimum motor power.
     */
    double get_min_motor_power();

    /**
     * @brief Sets the preset for auto mode.
     *
     * This function is responsible for setting the preset for auto mode.
     * In auto mode, the robot uses headless mode, does not use gyro support, and does not use centripetal force.
     * The time from zero to max speed is set to 100 milliseconds.
     */
    void preset_for_auto();

    /**
     * @brief Sets the preset for tele operation mode.
     *
     * This function is responsible for setting the preset for tele operation mode.
     * In tele operation mode, the robot uses headless mode, does not use gyro support, and uses centripetal force.
     * The time from zero to max speed is set to 100 milliseconds.
     */
    void preset_for_tele_op();
}


#endif //FTCROBOTCONTROLLER_DRIVETRAIN_H
