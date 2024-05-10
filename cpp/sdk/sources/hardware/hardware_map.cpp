//
// Created by fnlg on 28.12.2023.
//

#include "hardware/hardware_map.h"

using namespace std;


namespace sdk::hardware_map {
    jobject hardwareMap;

    jobject get(jclass jclazz, const string &device_name) {
        attach_thread
        jobject result = env->NewGlobalRef(
                env->CallObjectMethod(hardwareMap,
                                      env->GetMethodID(env->GetObjectClass(hardwareMap), "get",
                                                       "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Object;"),
                                      jclazz,
                                      env->NewStringUTF(device_name.c_str())));
        return result;
    }
}
// SDK