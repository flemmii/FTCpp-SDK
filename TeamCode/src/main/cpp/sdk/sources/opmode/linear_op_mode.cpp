//
// Created by fnlg on 29.12.2023.
//

#include "opmode/linear_op_mode.h"

namespace sdk {
    namespace linear_op_mode {
        jclass jclazz;
        jobject thiz;

        void wait_for_start() {
            attach_thread
            env->CallVoidMethod(thiz,
                                env->GetMethodID(jclazz, "waitForStart", "()V"));
        }

        void idle() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "idle", "()V"));
        }

        void sleep(int milliseconds) {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "sleep", "(J)V"),
                                static_cast<jlong> (milliseconds));
        }

        bool op_mode_is_active() {
            attach_thread
            return env->CallBooleanMethod(thiz,
                                          env->GetMethodID(jclazz, "opModeIsActive", "()Z"));
        }

        bool op_mode_in_init() {
            attach_thread
            return env->CallBooleanMethod(thiz,
                                          env->GetMethodID(jclazz, "opModeInInit", "()Z"));
        }

        bool is_started() {
            attach_thread
            return env->CallBooleanMethod(thiz,
                                          env->GetMethodID(jclazz, "isStarted", "()Z"));
        }

        bool is_stop_requested() {
            attach_thread
            return env->CallBooleanMethod(thiz,
                                          env->GetMethodID(jclazz, "isStopRequested",
                                                           "()Z"));
        }

        void init() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "init", "()V"));
        }

        void init_loop() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "init_loop", "()V"));
        }

        void start() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "start", "()V"));
        }

        void loop() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "loop", "()V"));
        }

        void stop() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "stop", "()V"));
        }

        void request_op_mode_stop() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "requestOpModeStop", "()V"));
        }

        double get_runtime() {
            attach_thread
            return static_cast<double>(env->CallDoubleMethod(thiz,
                                                             env->GetMethodID(jclazz, "getRuntime",
                                                                              "()D")));
        }

        void reset_runtime() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "resetRuntime", "()V"));
        }

        void terminate_op_mode_now() {
            attach_thread
            env->CallVoidMethod(thiz, env->GetMethodID(jclazz, "terminateOpModeNow", "()V"));
        }
    }
}