#pragma once

#include "utils.hpp"

namespace sdk
{
    class Touch_sensor
    {
    public:
        static jclass jclazz;
        jobject touchSensor{};

        Touch_sensor() = default;

        Touch_sensor(const jobject &touchSensor);

        ~Touch_sensor();

        Touch_sensor &operator=(const Touch_sensor &touch_sensor);

        Touch_sensor &operator=(const jobject &touchSensor);

        [[nodiscard]] double get_value() const;

        [[nodiscard]] bool is_pressed() const;
    };

} // sdk
