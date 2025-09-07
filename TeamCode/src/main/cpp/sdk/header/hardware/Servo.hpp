#pragma once

#include <string>

#include "utils.hpp"
#include "Servo_controller.hpp"

namespace sdk
{
    class Servo
    {
    public:
        static jclass jclazz;
        static jclass jclazz_Direction;
        jobject servo{};

        double MIN_POSITION = 0.0;
        double MAX_POSITION = 1.0;

        enum class Direction
        {
            FORWARD,
            REVERSE
        };

        Servo() = default;

        Servo(const jobject &servo);

        ~Servo();

        Servo &operator=(const Servo &servo);

        virtual Servo &operator=(const jobject &servo);

        [[nodiscard]] Servo_controller get_controller() const;

        [[nodiscard]] int get_port_number() const;

        void set_direction(const Direction &direction) const;

        [[nodiscard]] Direction get_direction() const;

        static const char *direction_to_string(const Direction &direction);

        void set_position(const double &position) const;

        [[nodiscard]] double get_position() const;

        void scale_range(const double &min, const double &max);
    };

} // sdk
