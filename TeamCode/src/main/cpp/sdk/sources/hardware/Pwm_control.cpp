#include "hardware/Pwm_control.hpp"

using namespace std;

namespace sdk
{
    jclass Pwm_control::jclazz;
    jclass Pwm_control::Pwm_range::jclazz;
    // Define the static member outside the class
    const Pwm_control::Pwm_range Pwm_control::Pwm_range::default_range = Pwm_control::Pwm_range(Pwm_control::Pwm_range::us_pulse_lower_default, Pwm_control::Pwm_range::us_pulse_upper_default);

    Pwm_control::Pwm_control(const jobject &pwmControl) : pwmControl(pwmControl) {}

    Pwm_control::~Pwm_control()
    {
        if (pwmControl)
        {
            attach_thread
                env->DeleteGlobalRef(pwmControl);
            pwmControl = nullptr;
        }
    }

    Pwm_control &Pwm_control::operator=(const Pwm_control &Pwm_control)
    {
        if (this != &Pwm_control && Pwm_control.pwmControl)
        {
            attach_thread this->pwmControl = env->NewGlobalRef(Pwm_control.pwmControl);
        }
        return *this;
    }

    Pwm_control &Pwm_control::operator=(const jobject &pwmControl)
    {
        if (this->pwmControl)
        {
            attach_thread
                env->DeleteGlobalRef(this->pwmControl);
        }
        this->pwmControl = pwmControl;
        return *this;
    }

    void Pwm_control::set_pwm_range(const Pwm_control::Pwm_range &range)
    {
        attach_thread

            jobject jrange = env->NewObject(Pwm_control::Pwm_range::jclazz,
                                            env->GetMethodID(Pwm_control::Pwm_range::jclazz, "<init>", "(DDD)V"), static_cast<jdouble>(range.us_pulse_lower), static_cast<jdouble>(range.us_pulse_upper), static_cast<jdouble>(range.us_frame));

        env->CallVoidMethod(pwmControl, env->GetMethodID(jclazz, "setPwmRange", "(Lcom/qualcomm/robotcore/hardware/PwmControl$PwmRange;)V"), jrange);

        env->DeleteLocalRef(jrange);
    }

    Pwm_control::Pwm_range Pwm_control::get_pwm_range()
    {
        attach_thread

            jobject range = env->CallObjectMethod(pwmControl, env->GetMethodID(jclazz,
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

    void Pwm_control::set_pwm_enable()
    {
        attach_thread

            env->CallVoidMethod(pwmControl, env->GetMethodID(jclazz, "setPwmEnable", "()V"));
    }

    void Pwm_control::set_pwm_disable()
    {
        attach_thread

            env->CallVoidMethod(pwmControl, env->GetMethodID(jclazz, "setPwmDisable", "()V"));
    }

    bool Pwm_control::is_pwm_enabled()
    {
        attach_thread

            return env->CallBooleanMethod(pwmControl, env->GetMethodID(jclazz, "isPwmEnabled", "()Z"));
    }
} // sdk