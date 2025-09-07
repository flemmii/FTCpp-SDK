//
// Created by flemmi on 27.10.24.
//

#include "hardware/Servo_impl_ex.hpp"

namespace sdk
{
    jclass Servo_impl_ex::jclazz;

    Servo_impl_ex::Servo_impl_ex(const jobject &servoImplEx) : servoImplEx(servoImplEx),
                                                               Servo(servoImplEx),
                                                               Pwm_control(servoImplEx) {}

    Servo_impl_ex::~Servo_impl_ex()
    {
        if (servoImplEx)
        {
            attach_thread
                env->DeleteGlobalRef(servoImplEx);
            servoImplEx = nullptr;
        }

        Servo::servo = nullptr;
        Pwm_control::pwmControl = nullptr;
    }

    Servo_impl_ex &Servo_impl_ex::operator=(const Servo_impl_ex &servo_impl_ex)
    {
        if (this != &servo_impl_ex && servo_impl_ex.servoImplEx)
        {
            attach_thread this->servo = env->NewGlobalRef(servo_impl_ex.servoImplEx);
        }
        return *this;

        Servo::servo = servo_impl_ex.servoImplEx;
        Pwm_control::pwmControl = servo_impl_ex.servoImplEx;
        return *this;
    }

    Servo_impl_ex &Servo_impl_ex::operator=(const jobject &servoImplEx)
    {
        if (this->servoImplEx)
        {
            attach_thread
                env->DeleteGlobalRef(this->servoImplEx);
        }
        this->servoImplEx = servoImplEx;

        Servo::servo = servoImplEx;
        Pwm_control::pwmControl = servoImplEx;

        return *this;
    }

    void Servo_impl_ex::set_pwm_range(Pwm_control::Pwm_range range)
    {
        attach_thread

            jobject jrange = env->NewObject(Pwm_control::Pwm_range::jclazz,
                                            env->GetMethodID(Pwm_control::Pwm_range::jclazz, "<init>", "(DDD)V"), static_cast<jdouble>(range.us_pulse_lower), static_cast<jdouble>(range.us_pulse_upper), static_cast<jdouble>(range.us_frame));

        env->CallVoidMethod(servoImplEx, env->GetMethodID(jclazz, "setPwmRange", "(Lcom/qualcomm/robotcore/hardware/PwmControl$PwmRange;)V"), jrange);

        env->DeleteLocalRef(jrange);
    }

    Pwm_control::Pwm_range Servo_impl_ex::get_pwm_range()
    {
        attach_thread

            jobject range = env->CallObjectMethod(servoImplEx, env->GetMethodID(jclazz,
                                                                                "getPwmRange",
                                                                                "()Lcom/qualcomm/robotcore/hardware/PwmControl$PwmRange;"));
        auto us_pulse_lower = static_cast<double>(env->GetDoubleField(range,
                                                                      env->GetFieldID(Pwm_range::jclazz, "usPulseLower",
                                                                                      "D")));
        auto us_pulse_upper = static_cast<double>(env->GetDoubleField(range,
                                                                      env->GetFieldID(Pwm_range::jclazz, "usPulseUpper",
                                                                                      "D")));
        auto us_frame = static_cast<double>(env->GetDoubleField(range,
                                                                env->GetFieldID(Pwm_range::jclazz, "usFrame",
                                                                                "D")));

        env->DeleteLocalRef(range);

        return {us_pulse_lower, us_pulse_upper, us_frame};
    }

    void Servo_impl_ex::set_pwm_enable() const
    {
        attach_thread
            env->CallVoidMethod(servoImplEx, env->GetMethodID(jclazz, "setPwmEnable", "()V"));
    }

    void Servo_impl_ex::set_pwm_disable() const
    {
        attach_thread
            env->CallVoidMethod(servoImplEx, env->GetMethodID(jclazz, "setPwmDisable", "()V"));
    }

    bool Servo_impl_ex::is_pwm_enabled() const
    {
        attach_thread return env->CallBooleanMethod(servoImplEx, env->GetMethodID(jclazz, "isPwmEnabled", "()Z"));
    }
} // sdk