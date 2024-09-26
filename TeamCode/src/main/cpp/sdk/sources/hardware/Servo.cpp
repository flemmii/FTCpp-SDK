//
// Created by fnlg on 27.01.2024.
//

#include "hardware/Servo.h"

using namespace std;

namespace sdk {
    jclass Servo::jclazz;
    jclass Servo::jclazz_Direction;

    Servo::Servo(const jobject &servo) : servo(servo) {}

    Servo::~Servo() {
        if (servo) {
            attach_thread
            env->DeleteGlobalRef(servo);
            servo = nullptr;
        }
    }

    class Servo &Servo::operator=(const jobject &servo) {
        if (this->servo) {
            attach_thread
            env->DeleteGlobalRef(this->servo);
        }
        this->servo = servo;
        return *this;
    }

    Servo_controller Servo::get_controller() const {
        attach_thread
        jobject jcontroller = env->CallObjectMethod(servo,
                                                    env->GetMethodID(jclazz, "getController",
                                                                     "()Lcom/qualcomm/robotcore/hardware/ServoController;"));

        Servo_controller controller(env->NewGlobalRef(jcontroller));
        env->DeleteLocalRef(jcontroller);
        return controller;
    }

    int Servo::get_port_number() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(servo,
                                                   env->GetMethodID(jclazz, "getPortNumber",
                                                                    "()I")));
    }

    void Servo::set_direction(const Direction &direction) const {
        attach_thread
        jobject jdirection = env->GetStaticObjectField(jclazz_Direction,
                                                       env->GetStaticFieldID(jclazz_Direction,
                                                                             direction_to_string(
                                                                                     direction),
                                                                             "Lcom/qualcomm/robotcore/hardware/Servo$Direction;"));
        env->CallVoidMethod(servo, env->GetMethodID(jclazz, "setDirection",
                                                    "(Lcom/qualcomm/robotcore/hardware/Servo$Direction;)V"),
                            jdirection);
        env->DeleteLocalRef(jdirection);
    }

    Servo::Direction Servo::get_direction() const {
        attach_thread

        jobject direction = env->CallObjectMethod(servo, env->GetMethodID(jclazz,
                                                                          "getDirection",
                                                                          "()Lcom/qualcomm/robotcore/hardware/Servo$Direction;"));


        auto name = (jstring) env->CallObjectMethod(direction,
                                                    env->GetMethodID(jclazz_Direction,
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *directionName = env->GetStringUTFChars(name, nullptr);
        std::string strDirectionName(directionName);
        env->DeleteLocalRef(name);
        env->DeleteLocalRef(direction);

        if (strDirectionName == "FORWARD")
            return Direction::FORWARD;
        else if (strDirectionName == "REVERSE")
            return Direction::REVERSE;
        return Direction::FORWARD;
    }

    const char *Servo::direction_to_string(const Direction &direction) {
        switch (direction) {
            case Direction::FORWARD:
                return "FORWARD";
            case Direction::REVERSE:
                return "REVERSE";
        }
    }

    void Servo::set_position(const double &position) const {
        attach_thread
        env->CallVoidMethod(servo, env->GetMethodID(jclazz, "setPosition", "(D)V"),
                            static_cast<jdouble> (position));
    }

    double Servo::get_position() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(servo,
                                                         env->GetMethodID(jclazz, "getPosition",
                                                                          "()D")));
    }

    void Servo::scale_range(const double &min, const double &max) {
        attach_thread
        env->CallVoidMethod(servo, env->GetMethodID(jclazz, "scaleRange", "(DD)V"),
                            static_cast<jdouble>(min), static_cast<jdouble>(max));
        MIN_POSITION = min;
        MAX_POSITION = max;
    }
} // sdk