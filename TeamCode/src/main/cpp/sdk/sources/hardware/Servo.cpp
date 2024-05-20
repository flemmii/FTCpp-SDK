//
// Created by fnlg on 27.01.2024.
//

#include "hardware/Servo.h"

using namespace std;

namespace sdk {
    jclass Servo;
    jclass Servo_Direction;

    Servo::Servo(jobject servo) : servo(servo) {}

    Servo::~Servo() {
        if (servo) {
            attach_thread
            env->DeleteGlobalRef(servo);
            servo = nullptr;
        }
    }

    class Servo &Servo::operator=(jobject servo) {
        if (this->servo) {
            attach_thread
            env->DeleteGlobalRef(this->servo);
        }
        this->servo = servo;
        return *this;
    }

    Servo_controller Servo::get_controller() const {
        attach_thread
        return {env->CallObjectMethod(servo, env->GetMethodID(sdk::Servo, "getController",
                                                              "()Lcom/qualcomm/robotcore/hardware/ServoController;"))};
    }

    int Servo::get_port_number() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(servo,
                                                   env->GetMethodID(sdk::Servo, "getPortNumber",
                                                                    "()I")));
    }

    void Servo::set_direction(Servo::Direction direction) {
        attach_thread
        jobject jdirection = env->GetStaticObjectField(sdk::Servo_Direction,
                                                       env->GetStaticFieldID(sdk::Servo_Direction,
                                                                             direction_to_string(
                                                                                     direction),
                                                                             "Lcom/qualcomm/robotcore/hardware/Servo$Direction;"));
        env->CallVoidMethod(servo, env->GetMethodID(sdk::Servo, "setDirection",
                                                    "(Lcom/qualcomm/robotcore/hardware/Servo$Direction;)V"),
                            jdirection);
        env->DeleteLocalRef(jdirection);
    }

    Servo::Direction Servo::get_direction() const {
        attach_thread

        jobject direction = env->CallObjectMethod(servo, env->GetMethodID(sdk::Servo,
                                                                          "getDirection",
                                                                          "()Lcom/qualcomm/robotcore/hardware/Servo$Direction;"));


        auto name = (jstring) env->CallObjectMethod(direction,
                                                    env->GetMethodID(Servo_Direction,
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

    const char *Servo::direction_to_string(Servo::Direction direction) {
        switch (direction) {
            case Direction::FORWARD:
                return "FORWARD";
            case Direction::REVERSE:
                return "REVERSE";
        }
    }

    void Servo::set_position(double position) const {
        attach_thread
        env->CallVoidMethod(servo, env->GetMethodID(sdk::Servo, "setPosition", "(D)V"),
                            static_cast<jdouble> (position));
    }

    double Servo::get_position() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(servo,
                                                         env->GetMethodID(sdk::Servo, "getPosition",
                                                                          "()D")));
    }

    void Servo::scale_range(double min, double max) {
        attach_thread
        env->CallVoidMethod(servo, env->GetMethodID(sdk::Servo, "scaleRange", "(D;D)V"),
                            static_cast<jdouble>(min), static_cast<jdouble>(max));
        MIN_POSITION = static_cast<double>(env->GetDoubleField(servo,
                                                               env->GetStaticFieldID(
                                                                       sdk::Servo,
                                                                       "MIN_POSITION", "D")));
        MAX_POSITION = static_cast<double>(env->GetDoubleField(servo,
                                                               env->GetStaticFieldID(
                                                                       sdk::Servo,
                                                                       "MAX_POSITION", "D")));
    }
} // sdk