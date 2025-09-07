#pragma once

#include "utils.hpp"

namespace sdk
{
    class Color_sensor
    {
    public:
        static jclass jclazz;
        jobject colorSensor{};

        Color_sensor() = default;

        Color_sensor(const jobject &colorSensor);

        ~Color_sensor();

        Color_sensor &operator=(const Color_sensor &color_sensor);

        Color_sensor &operator=(const jobject &colorSensor);

        [[nodiscard]] int red() const;

        [[nodiscard]] int green() const;

        [[nodiscard]] int blue() const;

        [[nodiscard]] int alpha() const;

        [[nodiscard]] std::tuple<int, int, int, int> argb() const;

        void enable_led(const bool &enable) const;
    };

} // sdk
