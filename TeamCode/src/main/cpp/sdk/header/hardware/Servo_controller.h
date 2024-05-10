//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_SERVO_CONTROLLER_H
#define FTCROBOTCONTROLLER_SERVO_CONTROLLER_H

#include "extras/utils.h"

namespace sdk {
    extern jclass ServoController;
    extern jclass ServoController_PwmStatus;

    class Servo_controller {
    public:
        jobject servoController{};

        Servo_controller() = default;

        Servo_controller(jobject servoController);

        ~Servo_controller();

        enum class Pwm_status {
            ENABLED, DISABLED, MIXED
        };

        void pwm_enable() const;

        void pwm_disable() const;

        [[nodiscard]] Pwm_status get_pwm_status() const;

        void set_servo_position(int servo, double position) const;

        [[nodiscard]] double get_servo_position(int servo) const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_SERVO_CONTROLLER_H
