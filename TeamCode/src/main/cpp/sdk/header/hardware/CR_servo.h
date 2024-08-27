//
// Created by fnlg on 10.05.2024.
//

#ifndef FTCROBOTCONTROLLER_CR_SERVO_H
#define FTCROBOTCONTROLLER_CR_SERVO_H

#include "utils.h"
#include "Servo_controller.h"
#include "Dc_motor_simple.h"

namespace sdk {
    class CR_servo : public Dc_motor_simple {
    public:
        static jclass jclazz;
        jobject crServo{};

        CR_servo() = default;

        CR_servo(const jobject &crServo);

        ~CR_servo();

        CR_servo &operator=(const jobject &crServo) override;

        [[nodiscard]] Servo_controller get_controller() const;

        [[nodiscard]] int get_port_number() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_CR_SERVO_H
