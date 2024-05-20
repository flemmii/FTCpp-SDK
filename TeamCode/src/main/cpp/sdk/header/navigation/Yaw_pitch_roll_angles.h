//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_YAW_PITCH_ROLL_ANGLES_H
#define FTCROBOTCONTROLLER_YAW_PITCH_ROLL_ANGLES_H

#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

#include "Angle_unit.h"

namespace sdk {

    struct Yaw_pitch_roll_angles {
    private:
        double yaw_degrees;
        double pitch_degrees;
        double roll_degrees;
        const long acquisition_time;

    public:
        Yaw_pitch_roll_angles(Angle_unit angle_unit, double yaw, double pitch, double roll,
                              long acquisition_time);

        double get_yaw(Angle_unit angle_unit);

        double get_pitch(Angle_unit angle_unit);

        double get_roll(Angle_unit angle_unit);

        long get_acquisition_time();

        std::string to_string();
    };

} // sdk

#endif //FTCROBOTCONTROLLER_YAW_PITCH_ROLL_ANGLES_H
