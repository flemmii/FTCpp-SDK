//
// Created by fnlg on 28.12.2023.
//

#include "hardware/hardware_map.h"

using namespace std;


namespace sdk::hardware_map {
    jobject hardwareMap;

    jobject get(jclass jclazz, const string &device_name) {
        attach_thread
        jclass HardwareMap = env->GetObjectClass(hardwareMap);
        jstring jstr = env->NewStringUTF(device_name.c_str());

        jobject device =
                env->CallObjectMethod(hardwareMap,
                                      env->GetMethodID(HardwareMap, "get",
                                                       "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Object;"),
                                      jclazz,
                                      jstr);

        jobject result = env->NewGlobalRef(device);

        env->DeleteLocalRef(HardwareMap);
        env->DeleteLocalRef(jstr);
        env->DeleteLocalRef(device);
        return result;
    }
}
// SDK