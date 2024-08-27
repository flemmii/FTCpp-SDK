//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_SERVO_H
#define FTCROBOTCONTROLLER_SERVO_H

#include <string>

#include "utils.h"
#include "Servo_controller.h"

namespace sdk {
    class Servo {
    public:
        static jclass jclazz;
        static jclass jclazz_Direction;
        jobject servo{};

        double MIN_POSITION = 0.0;
        double MAX_POSITION = 1.0;

        enum class Direction {
            FORWARD, REVERSE
        };

        Servo() = default;

        Servo(const jobject &servo);

        ~Servo();

        Servo &operator=(const jobject &servo);

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

#endif //FTCROBOTCONTROLLER_SERVO_H
