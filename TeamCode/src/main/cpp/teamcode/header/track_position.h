//
// Created by fnlg on 01.01.2024.
//

#ifndef FTCROBOTCONTROLLER_TRACK_POSITION_H
#define FTCROBOTCONTROLLER_TRACK_POSITION_H

#include <thread>
#include <numeric>
#include <vector>

#include "sdk/hardware/hardware_map.h"
#include "sdk/hardware/Dc_motor_ex.h"
#include "sdk/opmode/linear_op_mode.h"

#include "Position.h"
#include "robot.h"

#define ENCODER_TICKS_PER_ROTATION 8192
#define WHEEL_DIAMETER_MM 60


namespace track_position {
    // Tracks the position of the robot in x, y and a continuous rotation
    // system. It also calculates some other data about the robots
    // movement. All these data is stored in the variables
    void init();

    // Used to set the robots position to a specific position
    void set_position(double x_position, double y_position, double rotation);

    void set_position(Position position);

    void set_coordinates(double x_position, double y_position);

    void set_rotation(double rotation);

    Position get_position();

    void log_current_position();

    Vec2d get_coordinates();

    double get_forward_mm();

    double get_sidewards_mm();

    // Use this only with cosinus and sinus
    double get_rotation_rad();

    // In mm/s
    Vec2d get_speed();

    double get_extra_degrees();

    int get_loop_time();
}

#endif //FTCROBOTCONTROLLER_TRACK_POSITION_H
