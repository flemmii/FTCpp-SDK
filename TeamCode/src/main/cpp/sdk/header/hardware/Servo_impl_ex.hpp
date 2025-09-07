#pragma once

#include "utils.hpp"
#include "hardware/Servo.hpp"
#include "hardware/Pwm_control.hpp"

namespace sdk
{
    class Servo_impl_ex : public Servo, Pwm_control
    {
    public:
        static jclass jclazz;
        jobject servoImplEx{};

        Servo_impl_ex() = default;

        Servo_impl_ex(const jobject &servoImplEx);

        ~Servo_impl_ex();

        Servo_impl_ex &operator=(const Servo_impl_ex &servo_impl_ex);

        Servo_impl_ex &operator=(const jobject &servoImplEx) override;

        void set_pwm_range(Pwm_control::Pwm_range range);

        Pwm_control::Pwm_range get_pwm_range();

        void set_pwm_enable() const;

        void set_pwm_disable() const;

        [[nodiscard]] bool is_pwm_enabled() const;
    };
}
