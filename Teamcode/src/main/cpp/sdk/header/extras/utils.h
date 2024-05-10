//
// Created by fnlg on 09.05.2024.
//

#ifndef FTCROBOTCONTROLLER_UTILS_H
#define FTCROBOTCONTROLLER_UTILS_H

#include <chrono>
#include <android/log.h>
#include <jni.h>

#include "opmode/linear_op_mode.h"

// Macro to attach the current thread to the JVM
#define attach_thread   JNIEnv *env; \
                        sdk::jvm->AttachCurrentThread(reinterpret_cast<JNIEnv **> (&env), nullptr);

void logcat_log(int ERROR_LEVEL, const char *text, ...);

void sleep(const std::chrono::duration<double> &duration);

// Namespace with everything needed to interact with the FTC Java SDK
namespace sdk {
    extern JavaVM *jvm;
}

#endif //FTCROBOTCONTROLLER_UTILS_H
