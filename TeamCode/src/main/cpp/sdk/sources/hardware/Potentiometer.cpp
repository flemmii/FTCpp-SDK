//
// Created by fnlg on 13.09.2024.
//

#include "hardware/Potentiometer.h"

namespace sdk {

    double Potentiometer::voltage_to_angle(const int &voltage) {
        double common_root = sqrt(363 - 440 * voltage + 400 * voltage * voltage);
        return ((-135 * (-33 * sqrt(3) + 20 * sqrt(3) * voltage - common_root)) /
                (11 * sqrt(3) + common_root));
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