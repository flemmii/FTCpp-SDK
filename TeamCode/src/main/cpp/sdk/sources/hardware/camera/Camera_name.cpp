//
// Created by Frosch PC - V2 on 05.02.2024.
//

#include "hardware/camera/Camera_name.h"

namespace sdk {
    jclass Camera_name::jclazz;

    Camera_name::Camera_name(const jobject &cameraName) : cameraName(cameraName) {}

    Camera_name::~Camera_name() {
        if (cameraName) {
            attach_thread
            env->DeleteGlobalRef(cameraName);
            cameraName = nullptr;
        }
    }

    Camera_name &Camera_name::operator=(const Camera_name &camera_name) {
        if (this != &camera_name && camera_name.cameraName) {
            attach_thread
            this->cameraName = env->NewGlobalRef(camera_name.cameraName);
        }
        return *this;
    }

    Camera_name &Camera_name::operator=(const jobject &cameraName) {
        if (this->cameraName) {
            attach_thread
            env->DeleteGlobalRef(this->cameraName);
        }
        this->cameraName = cameraName;
        return *this;
    }

    bool Camera_name::is_webcam() const {
        attach_thread
        return env->CallBooleanMethod(cameraName, env->GetMethodID(jclazz, "isWebcam", "()Z"));
    }

    bool Camera_name::is_camera_direction() const {
        attach_thread
        return env->CallBooleanMethod(cameraName,
                                      env->GetMethodID(jclazz, "isCameraDirection", "()Z"));
    }

    bool Camera_name::is_switchable() const {
        attach_thread
        return env->CallBooleanMethod(cameraName,
                                      env->GetMethodID(jclazz, "isSwitchable", "()Z"));
    }

    bool Camera_name::is_unknown() const {
        attach_thread
        return env->CallBooleanMethod(cameraName, env->GetMethodID(jclazz, "isUnknown", "()Z"));
    }
} // sdk