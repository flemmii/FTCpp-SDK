//
// Created by Frosch PC - V2 on 05.02.2024.
//

#include "hardware/camera/Webcam_name.h"

using namespace std;

namespace sdk {
    jclass Webcam_name::jclazz;

    Webcam_name::Webcam_name(const jobject &webcamName) : webcamName(webcamName),
                                                          Camera_name(webcamName) {}

    Webcam_name::~Webcam_name() {
        webcamName = nullptr;
    }

    Webcam_name &Webcam_name::operator=(const Webcam_name &webcam_name) {
        Camera_name::operator=(webcam_name);
        this->webcamName = Camera_name::cameraName;
        return *this;
    }

    Webcam_name &Webcam_name::operator=(const jobject &webcamName) {
        Camera_name::operator=(webcamName);
        this->webcamName = webcamName;
        return *this;
    }

    string Webcam_name::get_usb_device_name_if_attached() const {
        attach_thread
        auto jstr = (jstring) (env->CallObjectMethod(webcamName,
                                                     env->GetMethodID(jclazz,
                                                                      "getUsbDeviceNameIfAttached",
                                                                      "()Ljava/lang/String;")));
        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        string result(cstr);

        env->DeleteLocalRef(jstr);
        return result;
    }

    bool Webcam_name::is_attached() const {
        attach_thread
        return env->CallBooleanMethod(webcamName,
                                      env->GetMethodID(jclazz, "isAttached", "()Z"));
    }
}