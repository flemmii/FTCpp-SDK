#pragma once

#include <chrono>
#include <thread>
#include <android/log.h>
#include <jni.h>
#include <string>
#include <iostream>
#include <cmath>

#include "opmode/linear_op_mode.hpp"

// Macro to attach the current thread to the JVM
#define attach_thread \
    JNIEnv *env;      \
    sdk::jvm->AttachCurrentThread(reinterpret_cast<JNIEnv **>(&env), nullptr);

void logcat_log(const int &ERROR_LEVEL, const char *tag, const char *text, ...);

class LogcatBuffer : public std::streambuf
{
public:
    LogcatBuffer(int errorLevel, const std::string &tag);

    void setTag(const std::string &tag);

protected:
    virtual int overflow(int c) override;

private:
    int errorLevel;
    std::string tag;
    std::string buffer;
};

class LogcatStream : public std::ostream
{
public:
    LogcatStream(int errorLevel, const std::string &tag);
    ~LogcatStream();

private:
    LogcatBuffer *buffer;
};

LogcatStream log(int errorLevel, const std::string &tag);

jclass find_and_create_global_ref(JNIEnv *env, std::string name);

void sleep(const std::chrono::duration<double> &duration);

// Namespace with everything needed to interact with the FTC Java SDK
namespace sdk
{
    extern JavaVM *jvm;
}
