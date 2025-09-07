#pragma once

#include <jni.h>

#include "sdk.hpp"

namespace sdk
{
    // Method to initialize the C++ environment for the FTC Robot Controller
    // This includes getting the JavaVM, creating global references to various Java classes,
    // and initializing the linear_op_mode namespace.
    extern "C" void init_sdk(JNIEnv *env, jobject thiz);
}