//
// Created by fnlg on 28.12.2023.
//

#include "hardware/Dc_motor_ex.h"

using namespace std;

namespace sdk {
    jclass Dc_motor_ex::jclazz;
    jclass Dc_motor_ex::jclazz_RunMode;
    jclass Dc_motor_ex::jclazz_ZeroPowerBehavior;


    Dc_motor_ex::Dc_motor_ex(jobject dcMotorEx) : dcMotorEx(dcMotorEx),
                                                  Dc_motor_simple(dcMotorEx) {}

    Dc_motor_ex::~Dc_motor_ex() {
        dcMotorEx = nullptr;
    }

    Dc_motor_ex &Dc_motor_ex::operator=(jobject dcMotorEx) {
        Dc_motor_simple::operator=(dcMotorEx);
        this->dcMotorEx = dcMotorEx;
        return *this;
    }

    void Dc_motor_ex::set_motor_enable() const {
        attach_thread
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setMotorEnable", "()V"));
    }

    void Dc_motor_ex::set_motor_disable() const {
        attach_thread
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setMotorDisable", "()V"));
    }

    bool Dc_motor_ex::is_motor_enabled() const {
        attach_thread
        bool result = env->CallBooleanMethod(dcMotorEx,
                                             env->GetMethodID(jclazz, "isMotorEnabled", "()Z"));
        return result;
    }

    void Dc_motor_ex::set_velocity(double angularRate) const {
        attach_thread
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setVelocity", "(D)V"),
                            static_cast<jdouble>(angularRate));
    }

    double Dc_motor_ex::get_velocity() const {
        attach_thread
        auto result = static_cast<double>(env->CallDoubleMethod(dcMotorEx,
                                                                env->GetMethodID(jclazz,
                                                                                 "getVelocity",
                                                                                 "()D")));
        return result;
    }

    void
    Dc_motor_ex::set_PIDF_coefficients(Run_mode mode, PIDF_coefficients pid_coefficients) const {
        attach_thread
        jobject runMode = env->GetStaticObjectField(jclazz_RunMode,
                                                    env->GetStaticFieldID(jclazz_RunMode,
                                                                          run_mode_to_string(
                                                                                  mode),
                                                                          "Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;"));
        jobject pidfCoefficients = env->NewObject(PIDF_coefficients::jclazz,
                                                  env->GetMethodID(PIDF_coefficients::jclazz,
                                                                   "<init>",
                                                                   "(DDDD)V"),
                                                  pid_coefficients.p, pid_coefficients.i,
                                                  pid_coefficients.d, pid_coefficients.f);
        env->CallVoidMethod(dcMotorEx,
                            env->GetMethodID(jclazz, "setPIDFCoefficients",
                                             "(Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;Lcom/qualcomm/robotcore/hardware/PIDFCoefficients;)V"),
                            runMode,
                            pidfCoefficients);
        env->DeleteLocalRef(runMode);
        env->DeleteLocalRef(pidfCoefficients);
    }

    PIDF_coefficients Dc_motor_ex::get_PIDF_coefficients(Run_mode mode) const {
        attach_thread
        jobject RunMode = env->GetStaticObjectField(jclazz_RunMode,
                                                    env->GetStaticFieldID(
                                                            jclazz_RunMode,
                                                            run_mode_to_string(
                                                                    mode),
                                                            "Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;"));

        jobject coefficients = env->CallObjectMethod(dcMotorEx, env->GetMethodID(jclazz,
                                                                                 "getPIDFCoefficients",
                                                                                 "(Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;)Lcom/qualcomm/robotcore/hardware/PIDFCoefficients;"),
                                                     RunMode);

        auto p = static_cast<double> (env->GetDoubleField(coefficients,
                                                          env->GetFieldID(PIDF_coefficients::jclazz,
                                                                          "p",
                                                                          "D")));
        auto i = static_cast<double> (env->GetDoubleField(coefficients,
                                                          env->GetFieldID(PIDF_coefficients::jclazz,
                                                                          "i",
                                                                          "D")));
        auto d = static_cast<double> (env->GetDoubleField(coefficients,
                                                          env->GetFieldID(PIDF_coefficients::jclazz,
                                                                          "d",
                                                                          "D")));
        auto f = static_cast<double> (env->GetDoubleField(coefficients,
                                                          env->GetFieldID(PIDF_coefficients::jclazz,
                                                                          "f",
                                                                          "D")));

        env->DeleteLocalRef(RunMode);
        env->DeleteLocalRef(coefficients);

        return {p, i, d, f};
    }

    void Dc_motor_ex::set_target_position_tolerance(int tolerance) const {
        attach_thread
        env->CallVoidMethod(dcMotorEx,
                            env->GetMethodID(jclazz, "setTargetPositionTolerance", "(I)V"),
                            static_cast<jint>(tolerance));
    }

    int Dc_motor_ex::get_target_position_tolerance() const {
        attach_thread
        return static_cast<int> (env->CallIntMethod(dcMotorEx,
                                                    env->GetMethodID(jclazz,
                                                                     "getTargetPositionTolerance",
                                                                     "()I")));
    }

    bool Dc_motor_ex::is_over_current() const {
        attach_thread
        bool result = env->CallBooleanMethod(dcMotorEx,
                                             env->GetMethodID(jclazz, "isOverCurrent", "()Z"));
        return result;
    }

    int Dc_motor_ex::get_port_number() const {
        attach_thread
        int result = static_cast<int> (env->CallIntMethod(dcMotorEx, env->GetMethodID(jclazz,
                                                                                      "getPortNumber",
                                                                                      "()I")));
        return result;
    }

    void Dc_motor_ex::set_mode(Run_mode mode) const {
        attach_thread
        jobject runMode = env->GetStaticObjectField(jclazz_RunMode,
                                                    env->GetStaticFieldID(jclazz_RunMode,
                                                                          run_mode_to_string(
                                                                                  mode),
                                                                          "Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;"));
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setMode",
                                                        "(Lcom/qualcomm/robotcore/hardware/DcMotor$RunMode;)V"),
                            runMode);
        env->DeleteLocalRef(runMode);
    }

    Dc_motor_ex::Run_mode Dc_motor_ex::get_mode() const {
        attach_thread
        jobject mode = env->CallObjectMethod(dcMotorEx, env->GetMethodID(jclazz, "getMode",
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

    const char *Dc_motor_ex::run_mode_to_string(Run_mode mode) {
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

    void Dc_motor_ex::set_zero_power_behavior(Zero_power_behavior zero_power_behavior) const {
        attach_thread
        jobject zeroPowerBehavior = env->GetStaticObjectField(jclazz_ZeroPowerBehavior,
                                                              env->GetStaticFieldID(
                                                                      jclazz_ZeroPowerBehavior,
                                                                      zero_power_behavior_to_string(
                                                                              zero_power_behavior),
                                                                      "Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior;"));
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setZeroPowerBehavior",
                                                        "(Lcom/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior;)V"),
                            zeroPowerBehavior);
        env->DeleteLocalRef(zeroPowerBehavior);
    }

    Dc_motor_ex::Zero_power_behavior Dc_motor_ex::get_zero_power_behavior() const {
        attach_thread
        jobject zeroPowerBehavior = env->CallObjectMethod(dcMotorEx,
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
    Dc_motor_ex::zero_power_behavior_to_string(Zero_power_behavior zero_power_behavior) {
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

    void Dc_motor_ex::set_target_position(int position) const {
        attach_thread
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setTargetPosition", "(I)V"),
                            static_cast<jint> (position));
    }

    int Dc_motor_ex::get_target_position() const {
        attach_thread
        return static_cast<int> (env->CallIntMethod(dcMotorEx,
                                                    env->GetMethodID(jclazz,
                                                                     "getTargetPosition",
                                                                     "()I")));
    }

    bool Dc_motor_ex::is_busy() const {
        attach_thread
        bool result = env->CallBooleanMethod(dcMotorEx,
                                             env->GetMethodID(jclazz, "isBusy", "()Z"));
        return result;
    }

    int Dc_motor_ex::get_current_position() const {
        attach_thread
        int result = static_cast<int> (env->CallIntMethod(dcMotorEx, env->GetMethodID(jclazz,
                                                                                      "getCurrentPosition",
                                                                                      "()I")));
        return result;
    }
} // SDK