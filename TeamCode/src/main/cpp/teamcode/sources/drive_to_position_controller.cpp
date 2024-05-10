//
// Created by fnlg on 01.02.2024.
//

#include "../header/drive_to_position_controller.h"

using namespace std;
using namespace sdk;

namespace drive_to_position_controller {
    const int reverse_if_blue_alliance = red_alliance * 2 - 1;
    // TODO: These values are hardcoded, you can change them
    const Position max_deviation_to_assume_the_same_pos(100, 100, 20);
    const double estimated_deceleration = 1300; // In mm/s^2
    const double pathing_rotation_speed_multiplier = 0.4;
    const double percent_per_loop_multiplier = 2; // The higher the more precise but the longer the loops take
    const double target_power_subtraction = 0.1; // This gets subtracted from the min motor power in order to
    // find the target power at the target position (Range: 0 - ca. 0.3)

    // TODO: Tune PIDs
    const PID_controller pid_coordinates_driving(0.001, 0, 0.001);
    PID_controller pid_x_driving = pid_coordinates_driving;
    PID_controller pid_y_driving = pid_coordinates_driving;
    PID_controller pid_turn_driving(0.003, 0, 0.002);

    const PID_controller pid_coordinates_deceleration(0.005, 0, 0.0015);
    PID_controller pid_x_deceleration = pid_coordinates_deceleration;
    PID_controller pid_y_deceleration = pid_coordinates_deceleration;
    PID_controller pid_turn_deceleration(0.025, 0,
                                         0.0075); // WARNING! turn pid works differently than the coordinates pid

    Position starting_position(0, 0, 0);
    Position target_position(0, 0, 0);
    vector<Vec2d> control_points = {};
    auto start_time = chrono::high_resolution_clock::now();

    bool rotation_should_follow_path = false;
    bool arrive_forwards = true;
    bool position_reached = false;

    double percent_per_loop = 0;
    int timeout = 0;

    double min_motor_power = 0.25;
    double max_motor_power = 0.8;

    thread drive_to_position_thread;

    // Returns whether it was successful or not
    bool decelerate_controlled(double distance_to_target_needed,
                               chrono::high_resolution_clock::time_point start_time) {
        // TODO: Test the deceleration

        logcat_log(ANDROID_LOG_INFO, "decelerating now");

        pid_x_deceleration.reset();
        pid_y_deceleration.reset();
        pid_turn_deceleration.reset();

        // Capturing the values at the start of the deceleration as references
        Position cache = drivetrain::get_target_power();
        Vec2d original_power(cache.x, cache.y);
        double target_power = (drivetrain::get_min_motor_power() - target_power_subtraction);
        if (target_power < 0)
            target_power = 0;

        // TODO: Maybe this needs to be deleted
        if (fabs(original_power.x) < target_power)
            original_power.x = target_power;
        if (fabs(original_power.y) < target_power)
            original_power.y = target_power;

        Vec2d target_coordinates(target_position.x, target_position.y);
        Position current_position = track_position::get_position();
        Vec2d distance_to_target =
                target_coordinates - Vec2d(current_position.x, current_position.y);
        Vec2d original_distance_to_target = fabs(distance_to_target);

        logcat_log(ANDROID_LOG_DEBUG, "target power: %f", target_power);

        // Runs until we don't move anymore (or some other cases occur)
        while (fabs(distance_to_target.total_distance()) < distance_to_target_needed &&
               drivetrain::should_drive() && (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < timeout ||
                                              timeout == 0) &&
               !linear_op_mode::is_stop_requested()) {
            // TODO: comments
            current_position = track_position::get_position();
            distance_to_target = target_coordinates - Vec2d(current_position.x, current_position.y);

            Vec2d deceleration_power = (fabs(original_power) - target_power) *
                                       (distance_to_target / original_distance_to_target) +
                                       sgn(distance_to_target) * target_power;

            logcat_log(ANDROID_LOG_DEBUG, "distance to target: %f",
                       distance_to_target.total_distance());
            logcat_log(ANDROID_LOG_DEBUG, "current position x: %f, y: %f, rotation: %f",
                       current_position.x, current_position.y, current_position.rotation);

            if (isnan(deceleration_power.x))
                deceleration_power.x = 0;
            if (isnan(deceleration_power.y))
                deceleration_power.y = 0;
            logcat_log(ANDROID_LOG_DEBUG, "deceleration power x: %f, y: %f", deceleration_power.x,
                       deceleration_power.y);

            Vec2d speed = track_position::get_speed();
            logcat_log(ANDROID_LOG_DEBUG, "speed x: %f, y: %f", speed.x, speed.y);

            Vec2d deceleration(0, 1);
            if (speed.y != 0) {
                double x_y_ratio = fabs(speed.x / speed.y);
                deceleration = {
                        x_y_ratio * sqrt(1 + x_y_ratio * x_y_ratio) / (1 + x_y_ratio * x_y_ratio) *
                        estimated_deceleration,
                        sqrt(1 + x_y_ratio * x_y_ratio) / (1 + x_y_ratio * x_y_ratio) *
                        estimated_deceleration};
            }
            deceleration = sgn(speed) * deceleration;
            logcat_log(ANDROID_LOG_DEBUG, "deceleration x: %f, y: %f", deceleration.x,
                       deceleration.y);

            Vec2d calc_stop_point(target_coordinates);
            if (deceleration.x == 0)
                calc_stop_point.y =
                        speed.y * speed.y / (estimated_deceleration * 2) + current_position.y;
            else if (deceleration.y == 0)
                calc_stop_point.x =
                        speed.x * speed.x / (estimated_deceleration * 2) + current_position.x;
            else
                calc_stop_point = speed * speed / (deceleration * 2) +
                                  Vec2d(current_position.x, current_position.y);
            logcat_log(ANDROID_LOG_DEBUG, "calc stop point x: %f, y: %f",
                       calc_stop_point.x, calc_stop_point.y);

            Position calc_distance_to_target =
                    target_position - Position(calc_stop_point, current_position.rotation);
            Position corrective_power(
                    pid_x_deceleration.calculate_pid_algorithm(calc_distance_to_target.x),
                    pid_y_deceleration.calculate_pid_algorithm(calc_distance_to_target.y),
                    pid_turn_deceleration.calculate_pid_algorithm(
                            calc_distance_to_target.rotation));
            logcat_log(ANDROID_LOG_DEBUG, "corrective power x: %f, y: %f, rotation: %f",
                       corrective_power.x, corrective_power.y, corrective_power.rotation);

            Position total_power = corrective_power + deceleration_power;

            logcat_log(ANDROID_LOG_DEBUG, "total power x: %f, y: %f, rotation: %f",
                       total_power.x, total_power.y, total_power.rotation);

            // Headless needs to be activated!!!
            drivetrain::drive(total_power.y, total_power.x, total_power.rotation);
            drivetrain::do_drive();

            Position actual_power = drivetrain::get_target_power();
            logcat_log(ANDROID_LOG_DEBUG, "actual power x: %f, y: %f, rotation: %f", actual_power.x,
                       actual_power.y, actual_power.rotation);
            logcat_log(ANDROID_LOG_DEBUG, "------------------------");
        }

        // If the upper loop ended due to distancing from the target it will call the driving again
        // Probably we got pushed during deceleration
        if (fabs(distance_to_target.total_distance()) > distance_to_target_needed &&
            (chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time).count() < timeout ||
             timeout == 0) && !linear_op_mode::is_stop_requested()) {
            starting_position = current_position;
            control_points = {{starting_position.x, starting_position.y},
                              {target_position.x,   target_position.y}};
            rotation_should_follow_path = false;
            return false;
        }

        drivetrain::stop();
        return true;
    }

    // This handles all movements except movements with only rotation
    void drive_to_position() {
        // Declaration of variables needed later on
        double current_percent = 0;
        double distance_to_target_needed;
        position_reached = false;

        Vec2d last_bezier_point = control_points[0];
        vector<Vec2d> last_speed_points = {control_points[0], control_points[1]};
        vector<Vec2d> speed_points;

        int executed1 = 0;
        int executed2 = 0;

        // Resetting the PIDs
        pid_x_driving.reset();
        pid_y_driving.reset();
        pid_turn_driving.reset();

        // Sets all the support systems to the necessary options for this driving algorithm to work
        drivetrain::preset_for_auto();

        // This loop drives along the bezier curve until a certain distance to the target is reached
        while (!linear_op_mode::is_stop_requested() && (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < timeout ||
                                                        timeout == 0)) {

            // Initialization of our current position, current position is only set here
            Position current_position = track_position::get_position();

            logcat_log(ANDROID_LOG_DEBUG, "current position x: %f, y: %f, rotation: %f",
                       current_position.x, current_position.y, current_position.rotation);

            // Breaks this loop if the distance to the target gets low enough to decelerate
            Vec2d current_speed = track_position::get_speed();
            Vec2d deceleration(0, 1);
            if (current_speed.y != 0) {
                double x_y_ratio = fabs(current_speed.x / current_speed.y);
                deceleration = {
                        x_y_ratio * sqrt(1 + x_y_ratio * x_y_ratio) / (1 + x_y_ratio * x_y_ratio) *
                        estimated_deceleration,
                        sqrt(1 + x_y_ratio * x_y_ratio) / (1 + x_y_ratio * x_y_ratio) *
                        estimated_deceleration};
            }
            deceleration = sgn(current_speed) * deceleration;
            logcat_log(ANDROID_LOG_DEBUG, "deceleration x: %f, y: %f", deceleration.x,
                       deceleration.y);

            Vec2d calc_stop_point({target_position.x, target_position.y});
            if (deceleration.x == 0)
                calc_stop_point.y =
                        current_speed.y * current_speed.y / (estimated_deceleration * 2) +
                        current_position.y;
            else if (deceleration.y == 0)
                calc_stop_point.x =
                        current_speed.x * current_speed.x / (estimated_deceleration * 2) +
                        current_position.x;
            else
                calc_stop_point = current_speed * current_speed / (deceleration * 2) +
                                  Vec2d(current_position.x, current_position.y);
            logcat_log(ANDROID_LOG_DEBUG, "calc stop point x: %f, y: %f",
                       calc_stop_point.x, calc_stop_point.y);

            distance_to_target_needed =
                    (current_speed.x * current_speed.x + current_speed.y * current_speed.y) /
                    (2 * estimated_deceleration);

            if ((target_position - current_position).total_distance() < distance_to_target_needed &&
                (target_position - calc_stop_point).total_distance() < distance_to_target_needed)
                break;

            // Declaration of later used variable
            bool bezier_points_get_further_from_cur_pos = false;

            // Temporary variable to count the outer loops
            executed1++;

            // TODO: This could be done on compiling
            while (!bezier_points_get_further_from_cur_pos &&
                   !linear_op_mode::is_stop_requested() &&
                   (current_percent + percent_per_loop) < 1) {
                // Temporary variable to count the inner loops
                executed2++;
                // raising the current percent every loop so the point on the bezier can advance
                current_percent += percent_per_loop;

                vector<Vec2d> points = control_points;

                // looping until only one point is left -> This is our point on the bezier curve
                while (points.size() > 1 && !linear_op_mode::is_stop_requested()) {
                    // When there are only two points remaining, these two points are the speed points.
                    // A vector between them is the tangent of the bezier curve in the currently calculating point
                    if (points.size() == 2) {
                        speed_points = points;
                    }

                    // Vector to store the points calculated after this
                    vector<Vec2d> new_points;
                    new_points.reserve(points.size() - 1);

                    // Calculating all new points -> The new points size is the points size - 1
                    for (int i = 0; i < points.size() - 1; i++) {
                        new_points.emplace_back(
                                points[i].x + ((points[i + 1].x - points[i].x) * current_percent),
                                points[i].y + ((points[i + 1].y - points[i].y) * current_percent));
                    }
                    points = new_points;
                }

                // If the distance between our robot and the bezier point gets greater, we know our last point was the closest
                // to our robot and this point is also in front of our robot, so the robot won't drive backwards.
                // If this is not the case we store our current point and speed points and do another loop

                Vec2d current_coordinates(current_position.x, current_position.y);
                double distance_to_current_bezier_point = (points[0] -
                                                           current_coordinates).total_distance();
                if (distance_to_current_bezier_point >
                    (last_bezier_point - current_coordinates).total_distance() &&
                    distance_to_current_bezier_point >
                    (last_bezier_point - points[0]).total_distance()) {
                    bezier_points_get_further_from_cur_pos = true;
                    current_percent -= percent_per_loop;
                } else {
                    last_bezier_point = points[0];
                    last_speed_points = speed_points;
                }
            }

            // Declaration of some variables needed later on
            double current_target_rotation;
            double rotation_speed;

            Vec2d speed = last_speed_points[1] - last_speed_points[0];
            Vec2d would_be_next_speed = speed_points[1] - speed_points[0];

            double current_tangent_radians;

            // Tangent calculation: The tangent to our estimated position on the bezier curve
            if (speed.y != 0) {
                current_tangent_radians = atan2(speed.x, speed.y);
            } else {
                current_tangent_radians = (M_PI / 2) * ((speed.x > 0) - (speed.x < 0));
            }

            // Target rotation and rotation speed calculation:
            // If we should follow the path with our rotation, the rotation target is just the tangent degrees
            // and the rotation speed is the difference between our current target and our next target.
            // If we don't follow the path with our rotation, the rotation target is just our delta rotation
            // multiplied with the current percent and the rotation_speed is constant the delta rotation
            // multiplied with the percent per loop
            if (rotation_should_follow_path) {
                double rel_current_target_rotation =
                        current_tangent_radians - M_PI * !arrive_forwards;
                current_target_rotation =
                        round(current_position.rotation / 360 -
                              rel_current_target_rotation / (2 * M_PI)) *
                        (2 * M_PI) + rel_current_target_rotation;

                double would_be_next_tangent_degrees;
                if (would_be_next_speed.y != 0) {
                    would_be_next_tangent_degrees =
                            atan2(would_be_next_speed.x, would_be_next_speed.y);
                } else {
                    would_be_next_tangent_degrees = (M_PI / 2) * ((would_be_next_speed.x > 0) -
                                                                  (would_be_next_speed.x < 0));
                }

                double rel_would_be_next_target_rotation =
                        would_be_next_tangent_degrees - M_PI * !arrive_forwards;
                double would_be_next_target_rotation =
                        round(current_position.rotation / 360 -
                              rel_would_be_next_target_rotation / (2 * M_PI)) * (2 * M_PI) +
                        rel_would_be_next_target_rotation;

                rotation_speed = pathing_rotation_speed_multiplier *
                                 (would_be_next_target_rotation - current_target_rotation) *
                                 (180 / M_PI);
                current_target_rotation *= (180 / M_PI);
            } else {
                current_target_rotation = starting_position.rotation +
                                          (target_position.rotation -
                                           starting_position.rotation) * current_percent;

                rotation_speed = pathing_rotation_speed_multiplier *
                                 (target_position.rotation - starting_position.rotation) *
                                 percent_per_loop;
            }

            Position position_on_bezier = {last_bezier_point.x, last_bezier_point.y,
                                           current_target_rotation};
            logcat_log(ANDROID_LOG_DEBUG, "position on bezier x: %f, y: %f, turn: %f",
                       position_on_bezier.x, position_on_bezier.y, position_on_bezier.rotation);

            if (speed.x != 0 || speed.y != 0) {
                if (fabs(speed.x) > fabs(speed.y)) {
                    speed.y /= fabs(speed.x);
                    speed.x = ((speed.x > 0) - (speed.x < 0));
                } else {
                    speed.x /= fabs(speed.y);
                    speed.y = ((speed.y > 0) - (speed.y < 0));
                }
            }

            if (would_be_next_speed.x != 0 || would_be_next_speed.y != 0) {
                if (fabs(would_be_next_speed.x) > fabs(would_be_next_speed.y)) {
                    would_be_next_speed.y /= fabs(would_be_next_speed.x);
                    would_be_next_speed.x = ((would_be_next_speed.x > 0) -
                                             (would_be_next_speed.x < 0));
                } else {
                    would_be_next_speed.x /= fabs(would_be_next_speed.y);
                    would_be_next_speed.y = ((would_be_next_speed.y > 0) -
                                             (would_be_next_speed.y < 0));
                }
            }

            Vec2d acceleration = (would_be_next_speed - speed) * percent_per_loop_multiplier;

            // Centripetal force/acceleration calculation to stay on circular path
            double test = (speed.x * acceleration.y - speed.y * acceleration.x) * WEIGHT_OF_ROBOT /
                          speed.total_distance();

            double rotation_of_centripetal_force = current_tangent_radians + (M_PI / 2);

            Vec2d centripetal_force = {test * cos(rotation_of_centripetal_force),
                                       test * sin(rotation_of_centripetal_force)};
            logcat_log(ANDROID_LOG_DEBUG, "centripetal force x: %f, y: %f", centripetal_force.x,
                       centripetal_force.y);

            // Corrective speed calculation based on our current position and the optimal position
            Position corrective_speed = (position_on_bezier - current_position) * WEIGHT_OF_ROBOT;
            corrective_speed = {pid_x_driving.calculate_pid_algorithm(corrective_speed.x),
                                pid_y_driving.calculate_pid_algorithm(corrective_speed.y),
                                pid_turn_driving.calculate_pid_algorithm(
                                        corrective_speed.rotation)};
            logcat_log(ANDROID_LOG_DEBUG, "corrective_speed x: %f, y: %f, rotation: %f",
                       corrective_speed.x, corrective_speed.y, corrective_speed.rotation);

            // Pathing speed calculation: The higher pathing speed is always 1 or -1, the other
            // speed coordinate is in relation to the higher speed
            Position pathing_speed = {speed.x, speed.y, rotation_speed};
            logcat_log(ANDROID_LOG_DEBUG, "pathing_speed x: %f, y: %f, rotation: %f",
                       pathing_speed.x, pathing_speed.y, pathing_speed.rotation);

            // Total speed calculation. The higher the rank in this array the higher the priority. Also in the speeds
            // rotation has a higher priority than direction speed
            Position speeds[] = {{centripetal_force, 0}, corrective_speed, pathing_speed};
            Position total_speed(0, 0, 0);
            for (Position speed: speeds) {
                // TODO: Here are two variants of the prioritization. The upper one doesn't prioritize the rotation
                // TODO: speed, the lower does. Test both and look what's better (Florian prefers the lower one currently)

                /*
                power_remaining = max_motor_power - (fabs(total_speed.x) + fabs(total_speed.y) +
                                                     fabs(total_speed.rotation));
                percent_of_power =
                        power_remaining / (fabs(speed.x) + fabs(speed.y) + fabs(speed.rotation));

                if (percent_of_power > 1)
                    total_speed += speed;
                else {
                    total_speed += speed * percent_of_power;
                    break;
                }
                */

                double power_remaining = max_motor_power -
                                         (fabs(total_speed.x) + fabs(total_speed.y) +
                                          fabs(total_speed.rotation));
                double percent_of_power;

                if (speed.rotation != 0) {
                    percent_of_power = power_remaining / fabs(speed.rotation);

                    if (percent_of_power > 1)
                        total_speed.rotation += speed.rotation;
                    else {
                        total_speed.rotation += speed.rotation * percent_of_power;
                        break;
                    }
                    power_remaining += fabs(speed.rotation);
                }

                if (speed.x != 0 || speed.y != 0) {
                    percent_of_power = power_remaining / (fabs(speed.x) + fabs(speed.y));

                    Vec2d direction_speed(speed.x, speed.y);

                    if (percent_of_power > 1)
                        total_speed += direction_speed;
                    else {
                        total_speed += direction_speed * percent_of_power;
                        break;
                    }
                }
            }

            logcat_log(ANDROID_LOG_DEBUG, "total speed x: %f, y: %f, turn: %f",
                       total_speed.x, total_speed.y, total_speed.rotation);
            logcat_log(ANDROID_LOG_DEBUG, "---------------");

            // Headless needs to be activated!!!
            drivetrain::drive(total_speed.y, total_speed.x, total_speed.rotation);
            drivetrain::do_drive();
        }

        if (rotation_should_follow_path) {
            // Getting all necessary information
            double current_rotation = track_position::get_position().rotation;
            Vec2d last_speed = control_points.back() - control_points[control_points.size() - 2];

            // Tangent calculation: The tangent to the last control point
            double current_tangent_degrees;
            if (last_speed.y != 0) {
                current_tangent_degrees = atan2(last_speed.x, last_speed.y) * (180 / M_PI);
            } else {
                current_tangent_degrees = 90 * ((last_speed.x > 0) - (last_speed.x < 0));
            }

            // Calculating the final target rotation
            double rel_target_rotation = current_tangent_degrees - 180 * !arrive_forwards;
            target_position.rotation = round((current_rotation - rel_target_rotation) / 360) * 360 +
                                       rel_target_rotation;
        }

        if (!decelerate_controlled(distance_to_target_needed, start_time)) {
            drive_to_position();
        } else {
            logcat_log(ANDROID_LOG_DEBUG, "executed average: %f", (double) (executed2) / executed1);

            if (chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time).count() > timeout) {
                if (timeout == -1)
                    logcat_log(ANDROID_LOG_WARN, "drive to position got ended from outside");
                else
                    logcat_log(ANDROID_LOG_WARN, "drive to position ended due to timeout");
            } else {
                this_thread::sleep_for(chrono::milliseconds(1000));
                Position current_position = track_position::get_position();
                logcat_log(ANDROID_LOG_INFO, "end position x: %f, y: %f, rotation: %f",
                           current_position.x, current_position.y, current_position.rotation);
            }
        }
    }

    // This turns to a rotation if there is no (large) movement on the x and y axis
    void turn_to_rotation() {
        // Declaration of variables needed later on
        double rotation_to_target_needed;

        while (!linear_op_mode::is_stop_requested() && (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < timeout ||
                                                        timeout == 0)) {
            Position current_position = track_position::get_position();

            // Breaks this loop if the rotation to the target gets low enough to decelerate
            double current_speed = robot::imu.get_angular_velocity().z;
            rotation_to_target_needed = fabs(current_speed) /
                                        (2 * 360 * (estimated_deceleration / (TRACK_WIDTH * M_PI)));
            if (fabs(target_position.rotation - current_position.rotation) <
                rotation_to_target_needed)
                break;

            // Every speed is just handled as a corrective speed
            Position corrective_speed = target_position - current_position;
            Position speed(pid_x_driving.calculate_pid_algorithm(corrective_speed.x),
                           pid_y_driving.calculate_pid_algorithm(corrective_speed.y),
                           pid_turn_driving.calculate_pid_algorithm(corrective_speed.rotation));

            // Headless needs to be activated!!!
            drivetrain::drive(speed.y, speed.x, speed.rotation);
            drivetrain::do_drive();
        }

        if (!decelerate_controlled(max_deviation_to_assume_the_same_pos.total_distance(),
                                   start_time))
            drive_to_position();

        position_reached = true;

        if (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() > timeout) {
            if (timeout == -1)
                logcat_log(ANDROID_LOG_WARN, "turn to rotation got ended from outside");
            else
                logcat_log(ANDROID_LOG_WARN, "drive to position ended due to timeout");
        }
    }

    void hold_position() {
        while (!linear_op_mode::is_stop_requested() && (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < timeout ||
                                                        timeout == 0)) {
            Position current_position = track_position::get_position();

            // Every speed is just handled as a corrective speed
            Position corrective_speed = target_position - current_position;
            Position speed(pid_x_driving.calculate_pid_algorithm(corrective_speed.x),
                           pid_y_driving.calculate_pid_algorithm(corrective_speed.y),
                           pid_turn_driving.calculate_pid_algorithm(corrective_speed.rotation));

            // Headless needs to be activated!!!
            drivetrain::drive(speed.y, speed.x, speed.rotation);
            drivetrain::do_drive();
        }

        position_reached = true;

        if (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() > timeout) {
            if (timeout == -1)
                logcat_log(ANDROID_LOG_INFO, "hold position got ended from outside");
            else
                logcat_log(ANDROID_LOG_INFO, "hold position ended due to timeout");
        }
    }

    void drive_to_position(Position target_position, vector<Vec2d> control_points, int timeout) {
        drive_to_position_controller::target_position = target_position;
        rotation_should_follow_path = false;

        Position current_position = track_position::get_position();
        if (fabs(current_position - drive_to_position_controller::target_position) <
            max_deviation_to_assume_the_same_pos)
            starting_position = drive_to_position_controller::target_position;
        else
            starting_position = current_position;

        drive_to_position_controller::control_points.clear();
        drive_to_position_controller::control_points = std::move(control_points);
        drive_to_position_controller::control_points.insert(
                drive_to_position_controller::control_points.begin(),
                {drive_to_position_controller::target_position.x,
                 drive_to_position_controller::target_position.y});
        drive_to_position_controller::control_points.emplace_back(target_position.x,
                                                                  target_position.y);

        double avarage_distance_control_points = 0;
        for (int i = 0; i < drive_to_position_controller::control_points.size() - 1; i++) {
            avarage_distance_control_points += (
                    drive_to_position_controller::control_points[i + 1] -
                    drive_to_position_controller::control_points[i]).total_distance();
        }
        avarage_distance_control_points /= drive_to_position_controller::control_points.size();

        percent_per_loop =
                percent_per_loop_multiplier * (max_motor_power / avarage_distance_control_points);

        // This stops the old thread as soon as possible if there is one
        if (drive_to_position_thread.joinable()) {
            drive_to_position_controller::timeout = -1;
            drive_to_position_thread.join();
        }

        drive_to_position_controller::timeout = timeout;
        start_time = chrono::high_resolution_clock::now();

        if (timeout != 0) {
            if (fabs(starting_position.total_distance() - target_position.total_distance()) >
                max_deviation_to_assume_the_same_pos.total_distance())
                drive_to_position();
            else
                turn_to_rotation();
        } else {
            // If we just need to turn the normal drive to position won't work, that's why there is a custom turn
            // to rotation for such a case
            if (fabs(starting_position.total_distance() - target_position.total_distance()) >
                max_deviation_to_assume_the_same_pos.total_distance())
                drive_to_position_thread = thread(static_cast<void (*)()>(drive_to_position));
            else
                drive_to_position_thread = thread(turn_to_rotation);
        }
    }

    void drive_to_position(Position target_position, int timeout) {
        drive_to_position(target_position, {}, timeout);
    }

    void drive_to_position(double x, double y, double rotation, int timeout) {
        drive_to_position(Position(x, y, rotation), timeout);
    }

    void drive_to_position(vector<Vec2d> control_points, bool arrive_forwards, int timeout) {
        // Sets variables used while driving
        rotation_should_follow_path = true;
        drive_to_position_controller::arrive_forwards = arrive_forwards;

        // If we are too far away from our previous target position it wouldn't make sense to use it as
        // the starting position
        Position current_position = track_position::get_position();
        if (fabs(current_position - drive_to_position_controller::target_position) <
            max_deviation_to_assume_the_same_pos)
            starting_position = drive_to_position_controller::target_position;
        else
            starting_position = current_position;

        logcat_log(ANDROID_LOG_DEBUG, "current position y: %f, x: %f, rotation: %f",
                   current_position.y, current_position.x, current_position.rotation);
        logcat_log(ANDROID_LOG_DEBUG, "starting position y: %f, x: %f, rotation: %f",
                   starting_position.y, starting_position.x, starting_position.rotation);

        // This calculates a first guess of the target rotation but the target rotation will change later on
        // whilst the robot is driving
        Vec2d last_control_point = control_points.back();
        Vec2d last_speed = last_control_point - control_points[control_points.size() - 2];
        double rotation = 360 * (int) (current_position.rotation > 0 ? floor(
                current_position.rotation / 360) : ceil(current_position.rotation / 360));
        if (last_speed.y != 0) {
            rotation += tan(last_speed.x / last_speed.y) * (180 / M_PI);
        } else {
            rotation += 90 * ((last_speed.x > 0) - (last_speed.x < 0));
        }

        // This sets the target position and all control points
        drive_to_position_controller::target_position = {last_control_point.x, last_control_point.y,
                                                         rotation};
        drive_to_position_controller::control_points = std::move(control_points);
        drive_to_position_controller::control_points.insert(
                drive_to_position_controller::control_points.begin(),
                {starting_position.x, starting_position.y});
        logcat_log(ANDROID_LOG_DEBUG, "points x: %f, y: %f",
                   drive_to_position_controller::control_points[0].x,
                   drive_to_position_controller::control_points[0].y);

        // Through this you can figure out very imprecise how long the path the robot will drive is
        double avarage_distance_control_points = 0;
        for (int i = 0; i < drive_to_position_controller::control_points.size() - 1; i++) {
            avarage_distance_control_points += (
                    drive_to_position_controller::control_points[i + 1] -
                    drive_to_position_controller::control_points[i]).total_distance();
        }
        avarage_distance_control_points /= drive_to_position_controller::control_points.size();

        // This is to adjust the percent per loop to how fast we'll drive and how far we'll drive
        percent_per_loop =
                percent_per_loop_multiplier * (max_motor_power / avarage_distance_control_points);

        // This stops the old thread as soon as possible if there is one
        if (drive_to_position_thread.joinable()) {
            drive_to_position_controller::timeout = -1;
            drive_to_position_thread.join();
        }

        drive_to_position_controller::timeout = timeout;
        start_time = chrono::high_resolution_clock::now();

        if (timeout != 0) {
            drive_to_position();
        } else {
            drive_to_position_thread = thread(static_cast<void (*)()>(drive_to_position));
        }
    }

    void hold_position(Position position, int time) {
        target_position = position;

        // This stops the old thread as soon as possible if there is one
        if (drive_to_position_thread.joinable()) {
            drive_to_position_controller::timeout = -1;
            drive_to_position_thread.join();
        }

        timeout = time;
        start_time = chrono::high_resolution_clock::now();

        if (time != 0) {
            hold_position();
        } else {
            drive_to_position_thread = thread(static_cast<void (*)()>(hold_position));
        }
    }

    void set_timeout(int timeout) {
        drive_to_position_controller::timeout = timeout;
    }

    void stop() {
        timeout = -1;
    }

    void wait_until_position_is_reached(int timeout) {
        drive_to_position_controller::timeout = timeout;
        while (!position_reached);
    }

    bool is_position_reached() {
        return position_reached;
    }

    void set_max_motor_power(double max_motor_power) {
        drive_to_position_controller::max_motor_power = max_motor_power;
    }
} // drive_to_position_controller