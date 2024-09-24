//
// Created by fnlg on 13.09.2024.
//

#include "hardware/Potentiometer.h"

namespace sdk {

    double Potentiometer::voltage_to_angle(const double &voltage) {
        double common_root = sqrt(400 * voltage * voltage - 440 * voltage + 363);
        return -(135 * (-common_root + 20 * sqrt(3) * voltage - 33 * sqrt(3))) / (common_root + 11 * sqrt(3));
    }

    Potentiometer::Potentiometer(const jobject &potentiometer) : Analog_input(potentiometer) {}

    Potentiometer &Potentiometer::operator=(const jobject &potentiometer) {
        Analog_input::operator=(potentiometer);
        return *this;
    }

    double Potentiometer::get_angle() const {
        return voltage_to_angle(get_voltage());
    }

    double Potentiometer::get_max_angle() const {
        return voltage_to_angle(get_max_voltage());
    }
} // sdk