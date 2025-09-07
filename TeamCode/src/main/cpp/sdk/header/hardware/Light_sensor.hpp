#pragma once

#include <string>

#include "utils.hpp"

namespace sdk
{
    class Light_sensor
    {
    public:
        static jclass jclazz;
        jobject lightSensor{};

        Light_sensor() = default;

        Light_sensor(const jobject &lightSensor);

        ~Light_sensor();

        Light_sensor &operator=(const Light_sensor &light_sensor);

        Light_sensor &operator=(const jobject &lightSensor);

        /**
         * Get the amount of light detected by the sensor, scaled and cliped to a range
         * which is a pragmatically useful sensitivity. Note that returned values INCREASE as
         * the light energy INCREASES.
         * @return amount of light, on a scale of 0.0 to 1.0
         */
        [[nodiscard]] double get_light_detected() const;

        /**
         * Returns a signal whose strength is proportional to the intensity of the light measured.
         * Note that returned values INCREASE as the light energy INCREASES. The units in which
         * this signal is returned are unspecified.
         * @return a value proportional to the amount of light detected, in unspecified units
         */
        [[nodiscard]] double get_raw_light_detected() const;

        /**
         * Returns the maximum value that can be returned by {@link #getRawLightDetected}.
         * @return the maximum value that can be returned by getRawLightDetected
         * @see #getRawLightDetected
         */
        [[nodiscard]] double get_raw_light_detected_max() const;

        /**
         * Enable the LED light
         * @param enable true to enable; false to disable
         */
        void enable_led(const bool &enable) const;

        /**
         * Status of this sensor, in string form
         * @return status
         */
        [[nodiscard]] std::string status() const;
    };

} // sdk
