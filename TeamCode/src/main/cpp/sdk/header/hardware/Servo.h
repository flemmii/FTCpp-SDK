//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_SERVO_H
#define FTCROBOTCONTROLLER_SERVO_H

#include <string>

#include "utils.h"
#include "Servo_controller.h"

namespace sdk {
    extern jclass Servo;
    extern jclass Servo_Direction;

    class Servo {
        jobject servo{};
    public:
        double MIN_POSITION = 0.0;
        double MAX_POSITION = 1.0;

        enum class Direction {
            FORWARD, REVERSE
        };

        Servo() = default;

        Servo(jobject servo);

        ~Servo();

        Servo &operator=(jobject servo);

        [[nodiscard]] Servo_controller get_controller() const;

        [[nodiscard]] int get_port_number() const;

        void set_direction(Servo::Direction direction);

        [[nodiscard]] Direction get_direction() const;

        static const char *direction_to_string(Servo::Direction direction);

        void set_position(double position) const;

        [[nodiscard]] double get_position() const;

        void scale_range(double min, double max);
    };

} // sdk

#endif //FTCROBOTCONTROLLER_SERVO_H
