//
// Created by flemmi on 04.11.24.
//

#ifndef FTCROBOTCONTROLLER_COLOR_RANGE_SENSOR_H
#define FTCROBOTCONTROLLER_COLOR_RANGE_SENSOR_H

#include "hardware/Color_sensor.h"
#include "hardware/Distance_sensor.h"
#include "hardware/Light_sensor.h"

namespace sdk {
    class Color_range_sensor : public Color_sensor, public Distance_sensor, public Light_sensor {
    public:
        static jclass jclazz;

        Color_range_sensor() = default;

        Color_range_sensor(const jobject &color_range_sensor);

        ~Color_range_sensor() = default;

        Color_range_sensor &operator=(const Color_range_sensor &color_range_sensor) = default;

        Color_range_sensor &operator=(const jobject &color_range_sensor) override;
    };
} // sdk

#endif //FTCROBOTCONTROLLER_COLOR_RANGE_SENSOR_H
