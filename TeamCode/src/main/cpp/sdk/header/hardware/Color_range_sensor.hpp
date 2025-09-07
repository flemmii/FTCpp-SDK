#pragma once

#include "hardware/Color_sensor.hpp"
#include "hardware/Distance_sensor.hpp"
#include "hardware/Light_sensor.hpp"

namespace sdk
{
    class Color_range_sensor : public Color_sensor, public Distance_sensor, public Light_sensor
    {
    public:
        static jclass jclazz;

        Color_range_sensor() = default;

        Color_range_sensor(const jobject &color_range_sensor);

        ~Color_range_sensor();

        Color_range_sensor &operator=(const Color_range_sensor &color_range_sensor) = default;

        Color_range_sensor &operator=(const jobject &color_range_sensor) override;
    };
} // sdk
