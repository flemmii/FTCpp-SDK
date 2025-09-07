#pragma once

#include <string>

#include <dlfcn.h>
#include <iostream>
#include <jni.h>
#include <android/log.h>

extern "C" JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_sdk_LibLoader_run(JNIEnv *env, jclass clazz, jobject thiz);
