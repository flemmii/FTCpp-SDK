//
// Created by fnlg on 10.05.2024.
//

#include "hardware/CR_servo.h"

namespace sdk {
    jclass CR_servo::jclazz;

    CR_servo::CR_servo(const jobject &crServo) : crServo(crServo), Dc_motor_simple(crServo) {}

    CR_servo::~CR_servo() {
        crServo = nullptr;
    }

    CR_servo &CR_servo::operator=(const CR_servo &cr_servo) {
        Dc_motor_simple::operator=(cr_servo);
        this->crServo = Dc_motor_simple::dcMotorSimple;
        return *this;
    }

    CR_servo &CR_servo::operator=(const jobject &crServo) {
        Dc_motor_simple::operator=(crServo);
        this->crServo = crServo;
        return *this;
    }

    Servo_controller CR_servo::get_controller() const {
        attach_thread
        jobject jcontroller = env->CallObjectMethod(crServo,
                                                    env->GetMethodID(jclazz, "getController",
                                                                     "()com/qualcomm/robotcore/hardware/ServoController;"));

        Servo_controller controller(env->NewGlobalRef(jcontroller));
        env->DeleteLocalRef(jcontroller);
        return controller;
    }

    int CR_servo::get_port_number() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(crServo,
                                                   env->GetMethodID(jclazz, "getPortNumber",
                                                                    "()I")));
    }
} // sdk