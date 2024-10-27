//
// Created by flemmi on 27.10.24.
//

#ifndef FTCROBOTCONTROLLER_SERVO_IMPL_EX_H
#define FTCROBOTCONTROLLER_SERVO_IMPL_EX_H

#include "utils.h"
#include "hardware/Servo.h"

namespace sdk {
    class Servo_impl_ex : public Servo {
    public:
        static jclass jclazz;
        jobject servoImplEx{};

        Servo_impl_ex() = default;

        Servo_impl_ex(const jobject &servoImplEx);

        ~Servo_impl_ex();

        Servo_impl_ex &operator=(const jobject &servoImplEx) override;


        // TODO: Implement Pwm_range

        void set_pwm_enable() const;

        void set_pwm_disable() const;

        [[nodiscard]] bool is_pwm_enabled() const;
    };
}

#endif //FTCROBOTCONTROLLER_SERVO_IMPL_EX_H
