//
// Created by fnlg on 09.05.2024.
//

#include "utils.hpp"

using namespace std;

void logcat_log(const int &ERROR_LEVEL, const char *tag, const char *text, ...)
{
    va_list args;
    va_start(args, text);
    __android_log_vprint(ERROR_LEVEL, tag, text, args);
    va_end(args);
}

LogcatBuffer::LogcatBuffer(int errorLevel, const std::string &tag) : errorLevel(errorLevel), tag(tag) {}

void LogcatBuffer::setTag(const std::string &tag)
{
    this->tag = tag;
}

int LogcatBuffer::overflow(int c)
{
    if (c != EOF)
    {
        buffer += static_cast<char>(c);
        if (c == '\n')
        {
            logcat_log(errorLevel, tag.c_str(), "%s", buffer.c_str());
            buffer.clear();
        }
    }
    return c;
}

LogcatStream::LogcatStream(int errorLevel, const std::string &tag)
    : std::ostream(new LogcatBuffer(errorLevel, tag)), buffer(static_cast<LogcatBuffer *>(rdbuf())) {}

LogcatStream::~LogcatStream()
{
    delete buffer;
}

LogcatStream log(int errorLevel, const std::string &tag)
{
    return LogcatStream(errorLevel, tag);
}

jclass find_and_create_global_ref(JNIEnv *env, std::string name)
{
    jobject localRef = env->FindClass(name.c_str());
    jobject globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(localRef));
    env->DeleteLocalRef(localRef);
    return reinterpret_cast<jclass>(globalRef);
}

void sleep(const chrono::duration<double> &duration)
{
    constexpr int check_ms = 50;
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count();
    int execution_count = static_cast<int>(floor(milliseconds / check_ms));
    auto rest_time = chrono::milliseconds(milliseconds % check_ms);

    for (int i = 0; i < execution_count; i++)
    {
        if (sdk::linear_op_mode::is_stop_requested())
            return;

        this_thread::sleep_for(chrono::milliseconds(check_ms));
    }

    if (sdk::linear_op_mode::is_stop_requested())
        return;

    this_thread::sleep_for(rest_time);
}

namespace sdk
{
    JavaVM *jvm;
}