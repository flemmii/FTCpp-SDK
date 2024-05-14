//
// Created by fnlg on 10.05.2024.
//

#include "hardware/CR_servo.h"

namespace sdk {
    jclass CRServo;

    CR_servo::CR_servo(jobject crServo) : crServo(crServo), Dc_motor_simple(crServo) {}

    Servo_controller CR_servo::get_controller() const {
        attach_thread
        jobject jcontroller = env->CallObjectMethod(crServo,
                                                    env->GetMethodID(CRServo, "getController",
                                                                     "()com/qualcomm/robotcore/hardware/ServoController;"));

        Servo_controller controller(jcontroller);
        env->DeleteLocalRef(jcontroller);
        return controller;
    }

    int CR_servo::get_port_number() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(crServo,
                                                   env->GetMethodID(CRServo, "getPortNumber",
                                                                    "()I")));
    }
} // sdk