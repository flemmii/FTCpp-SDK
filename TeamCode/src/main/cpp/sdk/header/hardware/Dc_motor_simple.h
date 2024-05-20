//
// Created by fnlg on 10.05.2024.
//

#ifndef FTCROBOTCONTROLLER_DC_MOTOR_SIMPLE_H
#define FTCROBOTCONTROLLER_DC_MOTOR_SIMPLE_H

#include <string>

#include "utils.h"

namespace sdk {
    extern jclass DcMotorSimple;
    extern jclass DcMotorSimple_Direction;

    class Dc_motor_simple {
    public:
        jobject dcMotorSimple{};

        enum class Direction {
            FORWARD,
            REVERSE
        };

        Dc_motor_simple() = default;

        Dc_motor_simple(jobject dcMotorSimple);

        ~Dc_motor_simple();

        virtual Dc_motor_simple &operator=(jobject dcMotorSimple);

        void set_direction(Direction direction) const;

        [[nodiscard]] Direction get_direction() const;

        [[nodiscard]] static const char *direction_to_string(Direction direction);

        void set_power(double power) const;

        [[nodiscard]] double get_power() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_DC_MOTOR_SIMPLE_H
