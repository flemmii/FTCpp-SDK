#pragma once

#include "utils.hpp"

namespace sdk
{
    class Servo_controller
    {
    public:
        static jclass jclazz;
        static jclass jclazz_PwmStatus;
        jobject servoController{};

        Servo_controller() = default;

        Servo_controller(const jobject &servoController);

        ~Servo_controller();

        Servo_controller &operator=(const Servo_controller &servo_controller);

        Servo_controller &operator=(const jobject &servoController);

        enum class Pwm_status
        {
            ENABLED,
            DISABLED,
            MIXED
        };

        void pwm_enable() const;

        void pwm_disable() const;

        [[nodiscard]] Pwm_status get_pwm_status() const;

        void set_servo_position(const int &servo, const double &position) const;

        [[nodiscard]] double get_servo_position(const int &servo) const;
    };

} // sdk
