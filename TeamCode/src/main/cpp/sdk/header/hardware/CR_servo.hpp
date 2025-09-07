#pragma once

#include "utils.hpp"
#include "Servo_controller.hpp"
#include "Dc_motor_simple.hpp"

namespace sdk
{
    class CR_servo : public Dc_motor_simple
    {
    public:
        static jclass jclazz;
        jobject crServo{};

        CR_servo() = default;

        CR_servo(const jobject &crServo);

        ~CR_servo();

        CR_servo &operator=(const CR_servo &cr_servo);

        CR_servo &operator=(const jobject &crServo) override;

        [[nodiscard]] Servo_controller get_controller() const;

        [[nodiscard]] int get_port_number() const;
    };

} // sdk
