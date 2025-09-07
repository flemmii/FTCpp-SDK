#pragma once

#include "utils/Location.hpp"
#include "subsystems/driving/drivetrain.hpp"
#include "robot.hpp"

/**
 * @namespace drive_to_position_controller
 * @brief A namespace containing functions for controlling the robot's movement to a target position.
 *
 * This namespace includes functions for driving the robot to a target position, holding the robot's current position,
 * setting a timeout for the robot's movement, stopping the robot's movement, waiting until the robot has reached its target position,
 * checking if the robot has reached its target position, and setting the maximum power for the robot's motors.
 *
 * The functions in this namespace use a PID controller to adjust the robot's movement and ensure it follows the path accurately.
 * They also handle the robot's deceleration as it approaches the target position to ensure a smooth stop.
 */
namespace drive_to_position_controller
{
    // Maximal deceleration that can be given without the robot slipping
    constexpr ::Acceleration MAX_DECELERATION = drivetrain::STATIC_FRICTION / robot::MASS;
    // Deceleration applied to the robot by the friction in the drivetrain
    constexpr ::Acceleration DECELERATION_FRICTION = drivetrain::FRICTION / robot::MASS;

    extern const Location<Millimetre, Degree> max_deviations;

    extern Power max_motor_power;
    extern double deceleration_multiplier;
    extern Location<> max_distance_from_target;

    extern bool position_reached;

    // These variables get their values from the predefine path methode
    extern std::vector<std::vector<Vec2d<>>> control_points_predefined_curves;
    extern std::vector<std::vector<Vec2d<>>> points_on_predefined_curves;
    extern std::vector<std::vector<std::tuple<Vec2d<>, Vec2d<>>>> speed_points_on_predefined_curves;

    void init();

    size_t predefine_path(std::vector<Vec2d<>> control_points);

    vector<size_t> predefine_sequential_paths(const vector<vector<Vec2d<>>> &paths);

    /**
     * @brief This function drives the robot to a target position following a path defined by control points.
     *
     * The function uses a PID controller to adjust the robot's movement and ensure it follows the path accurately.
     * It calculates the target speeds for the motors based on a bezier curve defined by the control points.
     *
     * The function also considers the centripetal force when calculating the robot's movement.
     * This is done to ensure the robot stays on the circular path of the bezier curve.
     *
     * The function also handles the robot's deceleration as it approaches the target position to ensure a smooth stop.
     * If the robot is pushed during deceleration, the function will call the driving again.
     *
     * @param target_position The target position for the robot.
     * @param control_points Different control points defining the path.
     * @param timeout The maximum time for the robot to reach the target position. 0 is asynchron execution. Default is 0.
     */
    void
    drive_to_position(Position<> target_position,
                      std::vector<Vec2d<>> control_points,
                      const chrono::steady_clock::duration &timeout = 0ms);

    /**
     * @brief This function drives the robot to a target position following a path defined by the target position.
     *
     * The function uses a PID controller to adjust the robot's movement and ensure it follows the path accurately.
     * It calculates the target speeds for the motors based on a bezier curve defined by the target position.
     *
     * The function also handles the robot's deceleration as it approaches the target position to ensure a smooth stop.
     * If the robot is pushed during deceleration, the function will call the driving again.
     *
     * @param target_position The target position for the robot.
     * @param timeout The maximum time for the robot to reach the target position. 0 is asynchron execution. Default is 0.
     */
    void drive_to_position(const Position<> &target_position, const chrono::steady_clock::duration &timeout = 0ms);

    /**
     * @brief This function drives the robot to a target position following a path defined by control points.
     *
     * The function uses a PID controller to adjust the robot's movement and ensure it follows the path accurately.
     * It calculates the target speeds for the motors based on a bezier curve defined by the control points.
     *
     * The function also considers the centripetal force when calculating the robot's movement.
     * This is done to ensure the robot stays on the circular path of the bezier curve.
     *
     * The function also handles the robot's deceleration as it approaches the target position to ensure a smooth stop.
     * If the robot is pushed during deceleration, the function will call the driving again.
     *
     * The rotation during the movement is tangential to the bezier curve.
     * By that the robot drives as fast as possible, because it only drives forwards/backwards and not sideways.
     *
     * @param control_points Different control points defining the path.
     * @param arrive_forwards If true, the robot arrives forwards. If false, the robot arrives backwards. Default is true.
     * @param timeout The maximum time for the robot to reach the target position. 0 is asynchron execution. Default is 0.
     */
    void
    drive_to_position(std::vector<Vec2d<>> control_points,
                      const bool &arrive_forwards = true,
                      const chrono::steady_clock::duration &timeout = 0ms);

    void drive_to_position(const size_t &index_of_path, Degree target_rotation,
                           const chrono::steady_clock::duration &timeout = 0ms);

    void drive_to_position(const size_t &index_of_path, const bool &arrive_forwards = true,
                           const chrono::steady_clock::duration &timeout = 0ms);

    void hold_position(Position<> position, const chrono::steady_clock::duration &time = 0ms);

    /**
     * @brief This function sets a timeout for the robot's movement.
     *
     * If the robot does not reach its target position within the specified timeout, the function will stop the robot's movement.
     *
     * The timeout is specified in milliseconds. A timeout of 0 means that the robot will continue moving until it reaches its target position,
     * regardless of how long it takes.
     *
     * If the timeout is set to -1, the function will stop the robot's movement as soon as possible.
     *
     * @param timeout The timeout for the robot's movement, in milliseconds. A value of 0 means no timeout. A value of -1 means stop as soon as possible.
     */
    void set_timeout(const chrono::steady_clock::duration &timeout);

    /**
     * @brief This function stops the robot's movement immediately.
     *
     * The function sends a command to the robot's motors to stop, halting any ongoing movement.
     * This includes any ongoing drive to position commands, as well as any other motor-related activities.
     * After calling this function, the robot will remain stationary until another command is issued.
     */
    void stop();

    /**
     * @brief This function waits until the robot has reached its target position or the timeout has elapsed.
     *
     * The function continuously checks if the robot has reached its target position. If the robot reaches its target position, the function returns immediately.
     *
     * If a timeout is specified, the function will return after the timeout has elapsed, even if the robot has not yet reached its target position.
     * A timeout of 0 means that the function will wait indefinitely until the robot reaches its target position.
     *
     * @param timeout The maximum time to wait for the robot to reach its target position, in milliseconds. A value of 0 means wait indefinitely.
     */
    void wait_until_position_is_reached(const chrono::steady_clock::duration &timeout);
} // drive_to_position_controller
