//
// Created by fnlg on 01.02.2024.
//

#ifndef FTCROBOTCONTROLLER_DRIVE_TO_POSITION_CONTROLLER_H
#define FTCROBOTCONTROLLER_DRIVE_TO_POSITION_CONTROLLER_H

#include <thread>
#include <vector>

#include "../sdk/header/telemetry.h"
#include "sdk/opmode/linear_op_mode.h"

#include "PID_controller.h"
#include "match_data.h"
#include "robot.h"
#include "Position.h"
#include "track_position.h"
#include "drivetrain.h"
#include "Vec2d.h"

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
namespace drive_to_position_controller {
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
    void drive_to_position(Position target_position, vector<Vec2d> control_points, int timeout = 0);

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
    void drive_to_position(Position target_position, int timeout = 0);

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
    drive_to_position(vector<Vec2d> control_points, bool arrive_forwards = true, int timeout = 0);

    /**
     * @brief This function makes the robot hold its current position for a specified amount of time.
     *
     * The function uses a PID controller to maintain the robot's current position.
     * It calculates the necessary motor speeds to counteract any forces that might cause the robot to move from its current position.
     *
     * If the robot is pushed or pulled during this time, the function will adjust the motor speeds to bring the robot back to its original position.
     *
     * The function can be used to make the robot hold its position indefinitely, or for a specified amount of time.
     * If a time is specified, the function will return after that time has elapsed, even if the robot has been moved from its original position.
     *
     * @param position The current position of the robot that should be held.
     * @param time The amount of time for which the robot should hold its position.
     * If 0, the robot will hold its position indefinitely until another function of this controller is called. Default is 0.
     */
    void drive_to_position(double x, double y, double rotation, int timeout = 0);
    // This drive_to_position function is the exact same as the other one, but with doubles instead of a Position object.

    void hold_position(Position position, int time = 0);

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
    void set_timeout(int timeout);

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
    void wait_until_position_is_reached(int timeout);

    /**
     * @brief This function checks if the robot has reached its target position.
     *
     * The function checks the current position of the robot and compares it with the target position.
     * If the robot's current position is the same as the target position, the function returns true.
     * Otherwise, it returns false.
     *
     * This function can be used to monitor the progress of the robot's movement towards its target position.
     *
     * @return true if the robot has reached its target position, false otherwise.
     */
    bool is_position_reached();

    /**
     * @brief This function sets the maximum power for the robot's motors.
     *
     * The function sets the maximum power that the robot's motors can use. This can be used to limit the speed of the robot.
     * The power is specified as a double, with 1.0 representing full power and 0.0 representing no power.
     *
     * @param max_motor_power The maximum power for the robot's motors, as a double between 0.0 and 1.0.
     */
    void set_max_motor_power(double max_motor_power);
} // drive_to_position_controller

#endif //FTCROBOTCONTROLLER_DRIVE_TO_POSITION_CONTROLLER_H
