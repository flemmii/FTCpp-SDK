//
// Created by fnlg on 28.12.2023.
//

#include "hardware/Dc_motor_ex.h"

using namespace std;

namespace sdk {
    jclass Dc_motor_ex::jclazz;
    jclass Dc_motor_ex::CurrentUnit;

    Dc_motor_ex::Dc_motor_ex(const jobject &dcMotorEx) : dcMotorEx(dcMotorEx),
                                                         Dc_motor(dcMotorEx) {}

    Dc_motor_ex::~Dc_motor_ex() {
        dcMotorEx = nullptr;
    }

    Dc_motor_ex &Dc_motor_ex::operator=(const Dc_motor_ex &dc_motor_ex) {
        Dc_motor::operator=(dc_motor_ex);
        this->dcMotorEx = Dc_motor::dcMotor;
        return *this;
    }

    Dc_motor_ex &Dc_motor_ex::operator=(const jobject &dcMotorEx) {
        Dc_motor::operator=(dcMotorEx);
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
        return env->CallBooleanMethod(dcMotorEx,
                                      env->GetMethodID(jclazz, "isMotorEnabled", "()Z"));
    }

    void Dc_motor_ex::set_velocity(const double &angular_rate) const {
        attach_thread
        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "setVelocity", "(D)V"),
                            static_cast<jdouble>(angular_rate));
    }

    double Dc_motor_ex::get_velocity() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(dcMotorEx,
                                                         env->GetMethodID(jclazz,
                                                                          "getVelocity",
                                                                          "()D")));
    }

    void
    Dc_motor_ex::set_PIDF_coefficients(const Run_mode &mode,
                                       const PIDF_coefficients &pid_coefficients) const {
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

    PIDF_coefficients Dc_motor_ex::get_PIDF_coefficients(const Run_mode &mode) const {
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

    void Dc_motor_ex::set_target_position_tolerance(const int &tolerance) const {
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

    double Dc_motor_ex::get_current() const {
        attach_thread
        jobject ampsObject = env->GetStaticObjectField(CurrentUnit,
                                                       env->GetStaticFieldID(CurrentUnit, "AMPS",
                                                                             "Lorg/firstinspires/ftc/robotcore/external/navigation/CurrentUnit;"));

        auto result = static_cast<double>(env->CallDoubleMethod(dcMotorEx,
                                                                env->GetMethodID(jclazz,
                                                                                 "getCurrent",
                                                                                 "(Lorg/firstinspires/ftc/robotcore/external/navigation/CurrentUnit;)D"),
                                                                ampsObject));
        env->DeleteLocalRef(ampsObject);
        return result;
    }

    double Dc_motor_ex::get_current_alert() const {
        attach_thread
        jobject ampsObject = env->GetStaticObjectField(CurrentUnit,
                                                       env->GetStaticFieldID(CurrentUnit, "AMPS",
                                                                             "Lorg/firstinspires/ftc/robotcore/external/navigation/CurrentUnit;"));

        auto result = static_cast<double>(env->CallDoubleMethod(dcMotorEx,
                                                                env->GetMethodID(jclazz,
                                                                                 "getCurrentAlert",
                                                                                 "(Lorg/firstinspires/ftc/robotcore/external/navigation/CurrentUnit;)D"),
                                                                ampsObject));
        env->DeleteLocalRef(ampsObject);
        return result;
    }

    void Dc_motor_ex::set_current_alert(const double &current) const {
        attach_thread
        jobject ampsObject = env->GetStaticObjectField(CurrentUnit,
                                                       env->GetStaticFieldID(CurrentUnit, "AMPS",
                                                                             "Lorg/firstinspires/ftc/robotcore/external/navigation/CurrentUnit;"));

        env->CallVoidMethod(dcMotorEx, env->GetMethodID(jclazz, "getCurrentAlert",
                                                        "(D;Lorg/firstinspires/ftc/robotcore/external/navigation/CurrentUnit;)V"),
                            static_cast<jdouble>(current), ampsObject);
        env->DeleteLocalRef(ampsObject);
    }

    bool Dc_motor_ex::is_over_current() const {
        attach_thread
        bool result = env->CallBooleanMethod(dcMotorEx,
                                             env->GetMethodID(jclazz, "isOverCurrent", "()Z"));
        return result;
    }
} // SDK