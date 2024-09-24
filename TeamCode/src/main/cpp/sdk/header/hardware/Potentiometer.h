//
// Created by fnlg on 13.09.2024.
//

#ifndef FTCROBOTCONTROLLER_POTENTIOMETER_H
#define FTCROBOTCONTROLLER_POTENTIOMETER_H

#include "Analog_input.h"

namespace sdk {
    class Potentiometer : public Analog_input {
    private:
        [[nodiscard]] static double voltage_to_angle(const double &voltage);

    public:
        Potentiometer() = default;

        Potentiometer(const jobject &potentiometer);

        Potentiometer &operator=(const jobject &potentiometer) override;

        [[nodiscard]] double get_angle() const;

        [[nodiscard]] double get_max_angle() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_POTENTIOMETER_H
