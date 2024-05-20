//
// Created by fnlg on 20.05.2024.
//

#include "navigation/Yaw_pitch_roll_angles.h"

namespace sdk {

    Yaw_pitch_roll_angles::Yaw_pitch_roll_angles(Angle_unit angle_unit, double yaw, double pitch,
                                                 double roll,
                                                 long acquisition_time) : acquisition_time(
            acquisition_time) {
        if (angle_unit == Angle_unit::DEGREES) {
            this->yaw_degrees = yaw;
            this->pitch_degrees = pitch;
            this->roll_degrees = roll;
        } else {
            this->yaw_degrees *= (180 / M_PI);
            this->pitch_degrees *= (180 / M_PI);
            this->roll_degrees *= (180 / M_PI);
        }
    }

    double Yaw_pitch_roll_angles::get_yaw(Angle_unit angle_unit) {
        if (angle_unit == Angle_unit::DEGREES)
            return yaw_degrees;
        else
            return yaw_degrees * (M_PI / 180);
    }

    double Yaw_pitch_roll_angles::get_pitch(Angle_unit angle_unit) {
        if (angle_unit == Angle_unit::DEGREES)
            return pitch_degrees;
        else
            return pitch_degrees * (M_PI / 180);
    }

    double Yaw_pitch_roll_angles::get_roll(Angle_unit angle_unit) {
        if (angle_unit == Angle_unit::DEGREES)
            return roll_degrees;
        else
            return roll_degrees * (M_PI / 180);
    }

    long Yaw_pitch_roll_angles::get_acquisition_time() {
        return acquisition_time;
    }

    std::string Yaw_pitch_roll_angles::to_string() {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << "{yaw=" << yaw_degrees << ", pitch="
            << pitch_degrees << ", roll=" << roll_degrees << "}";
        return oss.str();
    }
} // sdk