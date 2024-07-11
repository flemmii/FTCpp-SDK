//
// Created by fnlg on 10.05.2024.
//

#ifndef FTCROBOTCONTROLLER_COLOR_SENSOR_H
#define FTCROBOTCONTROLLER_COLOR_SENSOR_H

#include "utils.h"

namespace sdk {
    class Color_sensor {
    public:
        static jclass jclazz;
        jobject colorSensor{};

        Color_sensor() = default;

        Color_sensor(jobject colorSensor);

        ~Color_sensor();

        Color_sensor &operator=(jobject colorSensor);

        [[nodiscard]] int red() const;

        [[nodiscard]] int green() const;

        [[nodiscard]] int blue() const;

        [[nodiscard]] int alpha() const;

        [[nodiscard]] int argb() const;

        void enable_led(bool enable) const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_COLOR_SENSOR_H
