//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Dc_motor_simple.h"

namespace sdk {
    jclass DcMotorSimple;
    jclass DcMotorSimple_Direction;

    Dc_motor_simple::Dc_motor_simple(jobject dcMotorSimple) : dcMotorSimple(dcMotorSimple) {}

    void Dc_motor_simple::set_direction(Dc_motor_simple::Direction direction) const {
        attach_thread
        jobject jdirection = env->GetStaticObjectField(DcMotorSimple_Direction,
                                                       env->GetStaticFieldID(
                                                               DcMotorSimple_Direction,
                                                               direction_to_string(direction),
                                                               "Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;"));
        env->CallVoidMethod(dcMotorSimple, env->GetMethodID(DcMotorSimple, "setDirection",
                                                            "(Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;)V"),
                            jdirection);
        env->DeleteLocalRef(jdirection);
    }

    Dc_motor_simple::Direction Dc_motor_simple::get_direction() const {
        attach_thread

        jobject direction = env->CallObjectMethod(dcMotorSimple, env->GetMethodID(DcMotorSimple,
                                                                                  "getDirection",
                                                                                  "()Lcom/qualcomm/robotcore/hardware/DcMotorSimple$Direction;"));

        auto name = (jstring) env->CallObjectMethod(direction,
                                                    env->GetMethodID(env->GetObjectClass(direction),
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *directionName = env->GetStringUTFChars(name, nullptr);
        std::string strDirectionName(directionName);
        env->ReleaseStringUTFChars(name, directionName);

        env->DeleteLocalRef(direction);

        if (strDirectionName == "FORWARD")
            return Direction::FORWARD;
        else if (strDirectionName == "REVERSE")
            return Direction::REVERSE;
        return Direction::FORWARD;
    }

    const char *Dc_motor_simple::direction_to_string(Dc_motor_simple::Direction direction) {
        switch (direction) {
            case Direction::FORWARD:
                return "FORWARD";
            case Direction::REVERSE:
                return "REVERSE";
        }
    }

    void Dc_motor_simple::set_power(double power) const {
        attach_thread
        env->CallVoidMethod(dcMotorSimple, env->GetMethodID(DcMotorSimple, "setPower", "(D)V"),
                            static_cast<jdouble> (power));
    }

    double Dc_motor_simple::get_power() const {
        attach_thread
        auto result = static_cast<double> (env->CallDoubleMethod(dcMotorSimple,
                                                                 env->GetMethodID(DcMotorSimple,
                                                                                  "getPower",
                                                                                  "()D")));
        return result;
    }
} // sdk