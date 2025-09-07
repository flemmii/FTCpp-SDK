#pragma once

#include <string>

#include "utils.hpp"

namespace sdk
{
    class Dc_motor_simple
    {
    public:
        static jclass jclazz;
        static jclass jclazz_Direction;

        jobject dcMotorSimple{};

        enum class Direction
        {
            FORWARD,
            REVERSE
        };

        Dc_motor_simple() = default;

        Dc_motor_simple(const jobject &dcMotorSimple);

        ~Dc_motor_simple();

        Dc_motor_simple &operator=(const Dc_motor_simple &dc_motor_simple);

        virtual Dc_motor_simple &operator=(const jobject &dcMotorSimple);

        void set_direction(const Direction &direction) const;

        [[nodiscard]] Direction get_direction() const;

        [[nodiscard]] static const char *direction_to_string(const Direction &direction);

        void set_power(const double &power) const;

        [[nodiscard]] double get_power() const;
    };

} // sdk
