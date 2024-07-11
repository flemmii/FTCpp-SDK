//
// Created by fnlg on 27.01.2024.
//

#include "hardware/Servo_controller.h"

using namespace std;

namespace sdk {
    Servo_controller::Servo_controller(jobject servoController) : servoController(
            servoController) {}

    Servo_controller::~Servo_controller() {
        if (servoController) {
            attach_thread
            env->DeleteGlobalRef(servoController);
            servoController = nullptr;
        }
    }

    Servo_controller &Servo_controller::operator=(jobject servoController) {
        if (this->servoController) {
            attach_thread
            env->DeleteGlobalRef(this->servoController);
        }
        this->servoController = servoController;
        return *this;
    }


    void Servo_controller::pwm_enable() const {
        attach_thread
        env->CallVoidMethod(servoController, env->GetMethodID(jclazz, "pwmEnable", "()V"));
    }

    void Servo_controller::pwm_disable() const {
        attach_thread
        env->CallVoidMethod(servoController,
                            env->GetMethodID(jclazz, "pwmDisable", "()V"));
    }

    Servo_controller::Pwm_status Servo_controller::get_pwm_status() const {
        attach_thread
        jobject pwmStatus = env->CallObjectMethod(servoController,
                                                  env->GetMethodID(jclazz,
                                                                   "getPwmStatus",
                                                                   "()Lcom/qualcomm/robotcore/hardware/ServoController$PwmStatus;"));

        jclass PwmStatus = env->GetObjectClass(pwmStatus);

        auto name = (jstring) env->CallObjectMethod(pwmStatus,
                                                    env->GetMethodID(PwmStatus,
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *pwmStatusName = env->GetStringUTFChars(name, nullptr);
        std::string strPwmStatusName(pwmStatusName);
        env->DeleteLocalRef(name);

        env->DeleteLocalRef(PwmStatus);

        if (strPwmStatusName == "ENABLED")
            return Pwm_status::ENABLED;
        else if (strPwmStatusName == "DISABLED")
            return Pwm_status::DISABLED;
        else if (strPwmStatusName == "MIXED")
            return Pwm_status::MIXED;
        return Pwm_status::ENABLED;
    }

    void Servo_controller::set_servo_position(int servo, double position) const {
        attach_thread
        env->CallVoidMethod(servoController,
                            env->GetMethodID(jclazz, "setServoPosition", "(ID)V"),
                            static_cast<jint>(servo), static_cast<jdouble>(position));
    }

    double Servo_controller::get_servo_position(int servo) const {
        attach_thread
        return static_cast<jdouble> (env->CallDoubleMethod(servoController,
                                                           env->GetMethodID(jclazz,
                                                                            "getServoPosition",
                                                                            "(I)D"),
                                                           static_cast<jint>(servo)));
    }
} // sdk