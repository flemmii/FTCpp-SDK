#include "lib_loader.hpp"

#define LOG_TAG "lib_loader"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void *handle_sdk = nullptr;
void *handle_teamcode = nullptr;

typedef void (*FunctionType)();
typedef void (*FunctionTypeWithArgs)(JNIEnv *, jobject);
typedef void (*InitSdkFunctionType)(JNIEnv *, jobject);

std::string get_class_name(JNIEnv *env, jobject obj)
{
    jclass objClass = env->GetObjectClass(obj);
    jmethodID getClassMethod = env->GetMethodID(objClass, "getClass", "()Ljava/lang/Class;");
    jobject classObj = env->CallObjectMethod(obj, getClassMethod);

    jclass classClass = env->GetObjectClass(classObj);
    jmethodID getNameMethod = env->GetMethodID(classClass, "getSimpleName", "()Ljava/lang/String;");
    jstring classNameString = (jstring)env->CallObjectMethod(classObj, getNameMethod);

    const char *class_name_c_str = env->GetStringUTFChars(classNameString, nullptr);
    std::string class_name(class_name_c_str);
    env->ReleaseStringUTFChars(classNameString, class_name_c_str);

    return class_name;
}

void load_library(const char *lib_path, void **handle)
{
    LOGI("Attempting to load %s", lib_path);
    *handle = dlopen(lib_path, RTLD_NOW);
    if (!*handle)
    {
        LOGE("Cannot load %s: %s", lib_path, dlerror());
    }
    else
    {
        LOGI("%s loaded successfully", lib_path);
    }
    dlerror(); // Clear any existing error
}

void close_library(void *handle, const char *lib_name)
{
    if (handle)
    {
        dlclose(handle);
        LOGI("%s closed successfully", lib_name);
    }
    else
    {
        LOGE("Failed to close %s: handle is null", lib_name);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_sdk_LibLoader_run(JNIEnv *env, jclass clazz, jobject thiz)
{
    load_library("/data/local/tmp/libsdk.so", &handle_sdk);
    if (!handle_sdk)
        return;

    load_library("/data/local/tmp/libteamcode.so", &handle_teamcode);
    if (!handle_teamcode)
    {
        close_library(handle_sdk, "libsdk.so");
        return;
    }

    InitSdkFunctionType init_sdk = (InitSdkFunctionType)dlsym(handle_sdk, "init_sdk");
    const char *dlsym_error = dlerror();
    if (dlsym_error)
    {
        LOGE("Cannot load symbol 'init_sdk': %s", dlsym_error);
        close_library(handle_sdk, "libsdk.so");
        close_library(handle_teamcode, "libteamcode.so");
        return;
    }

    init_sdk(env, thiz);

    FunctionType op_mode = (FunctionType)dlsym(handle_teamcode, get_class_name(env, thiz).c_str());
    dlsym_error = dlerror();
    if (dlsym_error)
    {
        LOGE("Cannot load symbol 'op_mode': %s", dlsym_error);
        close_library(handle_sdk, "libsdk.so");
        close_library(handle_teamcode, "libteamcode.so");
        return;
    }

    op_mode();

    close_library(handle_sdk, "libsdk.so");
    close_library(handle_teamcode, "libteamcode.so");
}
