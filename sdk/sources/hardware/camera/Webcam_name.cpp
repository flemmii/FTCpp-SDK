//
// Created by Frosch PC - V2 on 05.02.2024.
//

#include "hardware/camera/Webcam_name.h"

using namespace std;

namespace sdk {
    jclass WebcamName;

    Webcam_name::Webcam_name(jobject webcamName) {
        attach_thread
        this->webcamName = env->NewGlobalRef(webcamName);
    }

    Webcam_name::~Webcam_name() {
        if (webcamName) {
            attach_thread
            env->DeleteGlobalRef(webcamName);
        }
    }

    string Webcam_name::get_usb_device_name_if_attached() const {
        attach_thread
        auto jstr = (jstring) (env->CallObjectMethod(webcamName,
                                                     env->GetMethodID(WebcamName,
                                                                      "getUsbDeviceNameIfAttached",
                                                                      "()Ljava/lang/String;")));
        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        string result(cstr);

        env->ReleaseStringUTFChars(jstr, cstr);
        return result;
    }

    bool Webcam_name::is_attached() const {
        attach_thread
        return env->CallBooleanMethod(webcamName,
                                      env->GetMethodID(WebcamName, "isAttached", "()Z"));
    }
}