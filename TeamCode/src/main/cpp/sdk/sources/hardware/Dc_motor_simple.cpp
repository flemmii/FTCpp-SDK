//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Dc_motor_simple.h"

namespace sdk {
    jclass Dc_motor_simple::jclazz;
    jclass Dc_motor_simple::jclazz_Direction;

    Dc_motor_simple::Dc_motor_simple(const jobject &dcMotorSimple) : dcMotorSimple(dcMotorSimple) {}

    Dc_motor_simple::~Dc_motor_simple() {
        if (dcMotorSimple) {
            attach_thread
            env->DeleteGlobalRef(dcMotorSimple);
            dcMotorSimple = nullptr;
        }
    }

    Dc_motor_simple &Dc_motor_simple::operator=(const jobject &dcMotorSimple) {
        if (this->dcMotorSimple) {
            attach_thread
            env->DeleteGlobalRef(this->dcMotorSimple);
        }
        this->dcMotorSimple = dcMotorSimple;
        return *this;
    }

    void Dc_motor_simple::set_direction(const Direction &direction) const {
        attach_thread
        jobject jdirection = env->GetStaticObjectField(jclazz_Direction,
                                                       env->GetStaticFieldID(
                                                               jclazz_Direction,
                                                               direction_to_string(direction),
                                                               "Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;"));
        env->CallVoidMethod(dcMotorSimple, env->GetMethodID(jclazz, "setDirection",
                                                            "(Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;)V"),
                            jdirection);
        env->DeleteLocalRef(jdirection);
    }

    Dc_motor_simple::Direction Dc_motor_simple::get_direction() const {
        attach_thread

        jobject direction = env->CallObjectMethod(dcMotorSimple, env->GetMethodID(jclazz,
                                                                                  "getDirection",
                                                                                  "()Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;"));

        jclass Direction = env->GetObjectClass(direction);

        auto name = (jstring) env->CallObjectMethod(direction,
                                                    env->GetMethodID(Direction,
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *directionName = env->GetStringUTFChars(name, nullptr);
        std::string strDirectionName(directionName);

        env->DeleteLocalRef(name);
        env->DeleteLocalRef(direction);
        env->DeleteLocalRef(Direction);

        if (strDirectionName == "FORWARD")
            return Direction::FORWARD;
        else if (strDirectionName == "REVERSE")
            return Direction::REVERSE;
        return Direction::FORWARD;
    }

    const char *Dc_motor_simple::direction_to_string(const Direction &direction) {
        switch (direction) {
            case Direction::FORWARD:
                return "FORWARD";
            case Direction::REVERSE:
                return "REVERSE";
        }
    }

    void Dc_motor_simple::set_power(const double &power) const {
        attach_thread
        env->CallVoidMethod(dcMotorSimple, env->GetMethodID(jclazz, "setPower", "(D)V"),
                            static_cast<jdouble> (power));
    }

    double Dc_motor_simple::get_power() const {
        attach_thread
        return static_cast<double> (env->CallDoubleMethod(dcMotorSimple,
                                                          env->GetMethodID(jclazz, "getPower",
                                                                           "()D")));
    }
} // sdk