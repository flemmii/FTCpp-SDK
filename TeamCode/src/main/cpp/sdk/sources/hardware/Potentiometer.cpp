//
// Created by fnlg on 13.09.2024.
//

#include "hardware/Potentiometer.h"

namespace sdk {

    double Potentiometer::voltage_to_angle(const double &voltage) {
        if (voltage < 0) {
            // returning a value even if there is a fault
            return 270;
        } else if (voltage < 1.200) {
            return 265.835 - (114.863 * voltage);
        } else if (voltage < 2.077) {
            return 222.438 - (76.143 * voltage);
        } else if (voltage < 3.26) {
            return (1000 - 260 * voltage) / (3 + 2 * voltage);
        } else {
            return 0;
        }
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