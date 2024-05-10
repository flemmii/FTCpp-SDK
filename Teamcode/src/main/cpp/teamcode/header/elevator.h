//
// Created by Frosch PC - V2 on 01.02.2024.
//

#ifndef FTCROBOTCONTROLLER_ELEVATOR_H
#define FTCROBOTCONTROLLER_ELEVATOR_H

#include "sdk/hardware/Dc_motor_ex.h"
#include "sdk/hardware/hardware_map.h"
#include "error_detection.h"

namespace elevator {
    void init();

    void set_target_power(double power);

    void set_target_elevation_in_mm(double elevation_in_mm, double power = 1);

    void check_encoder();

    double get_elevation_in_mm();

    double get_target_elevation_in_mm();

    double get_max_elevation();

    void stop();

    void set_target_elevation_to_layer(int layer);

    void set_target_elevation_to_layer();

    bool is_at_target_position();

    int get_layer();

    void set_layer(int layer);

    void deactivate();

    void set_power(double power);
} // elevator

#endif //FTCROBOTCONTROLLER_ELEVATOR_H
