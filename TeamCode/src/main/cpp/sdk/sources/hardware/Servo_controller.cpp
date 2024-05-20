//
// Created by fnlg on 27.01.2024.
//

#include "hardware/Servo_controller.h"

using namespace std;

namespace sdk {
    jclass ServoController;
    jclass ServoController_PwmStatus;

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
        env->CallVoidMethod(servoController, env->GetMethodID(ServoController, "pwmEnable", "()V"));
    }

    void Servo_controller::pwm_disable() const {
        attach_thread
        env->CallVoidMethod(servoController,
                            env->GetMethodID(ServoController, "pwmDisable", "()V"));
    }

    Servo_controller::Pwm_status Servo_controller::get_pwm_status() const {
        attach_thread
        switch (env->CallIntMethod(env->CallObjectMethod(servoController,
                                                         env->GetMethodID(ServoController,
                                                                          "getPwmStatus",
                                                                          "()com/qualcomm/hardware/ServoController$PwmStatus")),
                                   env->GetMethodID(ServoController_PwmStatus, "ordinal", "()I"))) {
            case 0:
                return Pwm_status::ENABLED;
            case 1:
                return Pwm_status::DISABLED;
            case 2:
                return Pwm_status::MIXED;
        }
        return Pwm_status::ENABLED;
    }

    void Servo_controller::set_servo_position(int servo, double position) const {
        attach_thread
        env->CallVoidMethod(servoController,
                            env->GetMethodID(ServoController, "setServoPosition", "(I;D)V"),
                            static_cast<jint>(servo), static_cast<jdouble>(position));
    }

    double Servo_controller::get_servo_position(int servo) const {
        attach_thread
        return static_cast<jdouble> (env->CallDoubleMethod(servoController,
                                                           env->GetMethodID(ServoController,
                                                                            "getServoPosition",
                                                                            "(I)V"),
                                                           static_cast<jint>(servo)));
    }
} // sdk