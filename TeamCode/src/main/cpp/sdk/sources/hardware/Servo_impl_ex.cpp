//
// Created by flemmi on 27.10.24.
//

#include "hardware/Servo_impl_ex.h"

namespace sdk {
    jclass Servo_impl_ex::jclazz;

    Servo_impl_ex::Servo_impl_ex(const jobject &servoImplEx) : servoImplEx(servoImplEx),
                                                               Servo(servoImplEx) {}

    Servo_impl_ex::~Servo_impl_ex() {
        servoImplEx = nullptr;
    }

    Servo_impl_ex &Servo_impl_ex::operator=(const jobject &servoImplEx) {
        Servo::operator=(servoImplEx);
        this->servoImplEx = servoImplEx;
        return *this;
    }

    void Servo_impl_ex::set_pwm_enable() const {
        attach_thread
        env->CallVoidMethod(servoImplEx, env->GetMethodID(jclazz, "setPwmEnable", "()V"));
    }

    void Servo_impl_ex::set_pwm_disable() const {
        attach_thread
        env->CallVoidMethod(servoImplEx, env->GetMethodID(jclazz, "setPwmDisable", "()V"));
    }

    bool Servo_impl_ex::is_pwm_enabled() const {
        attach_thread
        return env->CallBooleanMethod(servoImplEx, env->GetMethodID(jclazz, "isPwmEnabled", "()Z"));
    }
} // sdk