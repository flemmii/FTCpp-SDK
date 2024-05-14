//
// Created by fnlg on 29.12.2023.
//

#include "opmode/linear_op_mode.h"

namespace sdk {
    jobject currentOpMode;
    jclass CurrentOpMode;

    namespace linear_op_mode {

        void init_cpp() {
            attach_thread
            jobject localHardwareMap = env->GetObjectField(currentOpMode,
                                                           env->GetFieldID(
                                                                   CurrentOpMode,
                                                                   "hardwareMap",
                                                                   "Lcom/qualcomm/robotcore/hardware/HardwareMap;"));
            hardware_map::hardwareMap = env->NewGlobalRef(localHardwareMap);
            env->DeleteLocalRef(localHardwareMap);

            jobject localGamepad1 = env->GetObjectField(currentOpMode,
                                                        env->GetFieldID(
                                                                CurrentOpMode,
                                                                "gamepad1",
                                                                "Lcom/qualcomm/robotcore/hardware/Gamepad;"));
            gamepads::gamepad1 = {env->NewGlobalRef(localGamepad1)};
            env->DeleteLocalRef(localGamepad1);

            jobject localGamepad2 = env->GetObjectField(currentOpMode,
                                                        env->GetFieldID(
                                                                CurrentOpMode,
                                                                "gamepad2",
                                                                "Lcom/qualcomm/robotcore/hardware/Gamepad;"));
            gamepads::gamepad2 = {env->NewGlobalRef(localGamepad2)};
            env->DeleteLocalRef(localGamepad2);

            jobject localTelemetry = env->GetObjectField(currentOpMode,
                                                         env->GetFieldID(
                                                                 CurrentOpMode,
                                                                 "telemetry",
                                                                 "Lorg/firstinspires/ftc/robotcore/external/Telemetry;"));
            telemetry::telemetry = env->NewGlobalRef(localTelemetry);
            env->DeleteLocalRef(localTelemetry);
        }

        void wait_for_start() {
            attach_thread
            env->CallVoidMethod(currentOpMode,
                                env->GetMethodID(CurrentOpMode, "waitForStart", "()V"));
        }

        void idle() {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "idle", "()V"));
        }

        void sleep(int milliseconds) {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "sleep", "(J)V"),
                                static_cast<jlong> (milliseconds));
        }

        bool op_mode_is_active() {
            attach_thread
            return env->CallBooleanMethod(currentOpMode,
                                          env->GetMethodID(CurrentOpMode, "opModeIsActive", "()Z"));
        }

        bool op_mode_in_init() {
            attach_thread
            return env->CallBooleanMethod(currentOpMode,
                                          env->GetMethodID(CurrentOpMode, "opModeInInit", "()Z"));
        }

        bool is_started() {
            attach_thread
            return env->CallBooleanMethod(currentOpMode,
                                          env->GetMethodID(CurrentOpMode, "isStarted", "()Z"));
        }

        bool is_stop_requested() {
            attach_thread
            return env->CallBooleanMethod(currentOpMode,
                                          env->GetMethodID(CurrentOpMode, "isStopRequested",
                                                           "()Z"));
        }

        void init() {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "init", "()V"));
        }

        void init_loop() {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "init_loop", "()V"));
        }

        void start() {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "start", "()V"));
        }

        void loop() {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "loop", "()V"));
        }

        void stop() {
            attach_thread
            env->CallVoidMethod(currentOpMode, env->GetMethodID(CurrentOpMode, "stop", "()V"));
        }
    }
}