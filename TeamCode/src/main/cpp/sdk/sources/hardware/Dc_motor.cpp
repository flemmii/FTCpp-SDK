//
// Created by fnlg on 12.07.2024.
//

#include "hardware/Dc_motor.h"

using namespace std;

namespace sdk {
    jclass Dc_motor::jclazz;
    jclass Dc_motor::jclazz_RunMode;
    jclass Dc_motor::jclazz_ZeroPowerBehavior;

    Dc_motor::Dc_motor(const jobject &dcMotor) : dcMotor(dcMotor),
                                                 Dc_motor_simple(dcMotor) {}

    Dc_motor::~Dc_motor() {
        dcMotor = nullptr;
    }

    Dc_motor &Dc_motor::operator=(const jobject &dcMotor) {
        Dc_motor_simple::operator=(dcMotor);
        this->dcMotor = dcMotor;
        return *this;
    }

    int Dc_motor::get_port_number() const {
        attach_thread
        return static_cast<int> (env->CallIntMethod(dcMotor, env->GetMethodID(jclazz,
                                                                              "getPortNumber",
                                                                              "()I")));
    }

    void Dc_motor::set_mode(const Run_mode &mode) const {
        attach_thread
        jobject runMode = env->GetStaticObjectField(jclazz_RunMode,
                                                    env->GetStaticFieldID(jclazz_RunMode,
                                                                          run_mode_to_string(
                                                                                  mode),
                                                                          "Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;"));
        env->CallVoidMethod(dcMotor, env->GetMethodID(jclazz, "setMode",
                                                      "(Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;)V"),
                            runMode);
        env->DeleteLocalRef(runMode);
    }

    Dc_motor::Run_mode Dc_motor::get_mode() const {
        attach_thread
        jobject mode = env->CallObjectMethod(dcMotor, env->GetMethodID(jclazz, "getMode",
                                                                       "()Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;"));

        auto name = (jstring) env->CallObjectMethod(mode,
                                                    env->GetMethodID(jclazz_RunMode,
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *modeName = env->GetStringUTFChars(name, nullptr);
        std::string strModeName(modeName);
        env->DeleteLocalRef(name);

        env->DeleteLocalRef(mode);

        if (strModeName == "RUN_WITHOUT_ENCODER")
            return Run_mode::RUN_WITHOUT_ENCODER;
        else if (strModeName == "RUN_USING_ENCODER")
            return Run_mode::RUN_USING_ENCODER;
        else if (strModeName == "RUN_TO_POSITION")
            return Run_mode::RUN_TO_POSITION;
        else if (strModeName == "STOP_AND_RESET_ENCODER")
            return Run_mode::STOP_AND_RESET_ENCODER;
        return Run_mode::RUN_WITHOUT_ENCODER;
    }

    const char *Dc_motor::run_mode_to_string(const Run_mode &mode) {
        switch (mode) {
            case Run_mode::RUN_TO_POSITION:
                return "RUN_TO_POSITION";
            case Run_mode::RUN_USING_ENCODER:
                return "RUN_USING_ENCODER";
            case Run_mode::RUN_WITHOUT_ENCODER:
                return "RUN_WITHOUT_ENCODER";
            case Run_mode::STOP_AND_RESET_ENCODER:
                return "STOP_AND_RESET_ENCODER";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    void Dc_motor::set_zero_power_behavior(const Zero_power_behavior &zero_power_behavior) const {
        attach_thread
        jobject zeroPowerBehavior = env->GetStaticObjectField(jclazz_ZeroPowerBehavior,
                                                              env->GetStaticFieldID(
                                                                      jclazz_ZeroPowerBehavior,
                                                                      zero_power_behavior_to_string(
                                                                              zero_power_behavior),
                                                                      "Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior;"));
        env->CallVoidMethod(dcMotor, env->GetMethodID(jclazz, "setZeroPowerBehavior",
                                                      "(Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior;)V"),
                            zeroPowerBehavior);
        env->DeleteLocalRef(zeroPowerBehavior);
    }

    Dc_motor::Zero_power_behavior Dc_motor::get_zero_power_behavior() const {
        attach_thread
        jobject zeroPowerBehavior = env->CallObjectMethod(dcMotor,
                                                          env->GetMethodID(jclazz,
                                                                           "getZeroPowerBehavior",
                                                                           "()Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior;"));

        auto name = (jstring) env->CallObjectMethod(zeroPowerBehavior,
                                                    env->GetMethodID(jclazz_ZeroPowerBehavior,
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *zeroPowerBehaviorName = env->GetStringUTFChars(name, nullptr);
        std::string strZeroPowerBehaviorName(zeroPowerBehaviorName);
        env->DeleteLocalRef(name);

        env->DeleteLocalRef(zeroPowerBehavior);

        if (strZeroPowerBehaviorName == "BRAKE")
            return Zero_power_behavior::BRAKE;
        else if (strZeroPowerBehaviorName == "FLOAT")
            return Zero_power_behavior::FLOAT;
        else
            return Zero_power_behavior::UNKNOWN;
    }

    const char *
    Dc_motor::zero_power_behavior_to_string(const Zero_power_behavior &zero_power_behavior) {
        switch (zero_power_behavior) {
            case Zero_power_behavior::UNKNOWN:
                return "UNKNOWN";
            case Zero_power_behavior::BRAKE:
                return "BRAKE";
            case Zero_power_behavior::FLOAT:
                return "FLOAT";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    void Dc_motor::set_target_position(const int &position) const {
        attach_thread
        env->CallVoidMethod(dcMotor, env->GetMethodID(jclazz, "setTargetPosition", "(I)V"),
                            static_cast<jint> (position));
    }

    int Dc_motor::get_target_position() const {
        attach_thread
        return static_cast<int> (env->CallIntMethod(dcMotor,
                                                    env->GetMethodID(jclazz,
                                                                     "getTargetPosition",
                                                                     "()I")));
    }

    bool Dc_motor::is_busy() const {
        attach_thread
        return env->CallBooleanMethod(dcMotor,
                                      env->GetMethodID(jclazz, "isBusy", "()Z"));
    }

    int Dc_motor::get_current_position() const {
        attach_thread
        return static_cast<int> (env->CallIntMethod(dcMotor, env->GetMethodID(jclazz,
                                                                              "getCurrentPosition",
                                                                              "()I")));
    }
} // sdk