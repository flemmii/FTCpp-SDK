//
// Created by fnlg on 13.09.2024.
//

#ifndef FTCROBOTCONTROLLER_POTENTIOMETER_H
#define FTCROBOTCONTROLLER_POTENTIOMETER_H

#include "Analog_input.h"

namespace sdk {
    class Potentiometer : public Analog_input {
    private:
        /**
         * <p>
         * Formula based on experimental values
         * </p><p>
         * It is recommended to use the potentiometer in the range of 15 to 260 degrees anything beyond is absolutely not reliable
         * @param voltage
         * @return
         */
        [[nodiscard]] static double voltage_to_angle(const double &voltage);

    public:
        Potentiometer() = default;

        Potentiometer(const jobject &potentiometer);

        ~Potentiometer() = default;

        Potentiometer &operator=(const Potentiometer &potentiometer) = default;

        Potentiometer &operator=(const jobject &potentiometer) override;

        [[nodiscard]] double get_angle() const;

        [[nodiscard]] double get_max_angle() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_POTENTIOMETER_H
