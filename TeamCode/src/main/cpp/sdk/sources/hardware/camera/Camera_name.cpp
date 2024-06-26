//
// Created by Frosch PC - V2 on 05.02.2024.
//

#include "hardware/camera/Camera_name.h"

namespace sdk {
    jclass CameraName;

    Camera_name::Camera_name(jobject cameraName) : cameraName(cameraName) {}

    Camera_name::~Camera_name() {
        if (cameraName) {
            attach_thread
            env->DeleteGlobalRef(cameraName);
            cameraName = nullptr;
        }
    }

    Camera_name &Camera_name::operator=(jobject cameraName) {
        if (this->cameraName) {
            attach_thread
            env->DeleteGlobalRef(this->cameraName);
        }
        this->cameraName = cameraName;
        return *this;
    }

    bool Camera_name::is_webcam() const {
        attach_thread
        return env->CallBooleanMethod(cameraName, env->GetMethodID(CameraName, "isWebcam", "()Z"));
    }

    bool Camera_name::is_camera_direction() const {
        attach_thread
        return env->CallBooleanMethod(cameraName,
                                      env->GetMethodID(CameraName, "isCameraDirection", "()Z"));
    }

    bool Camera_name::is_switchable() const {
        attach_thread
        return env->CallBooleanMethod(cameraName,
                                      env->GetMethodID(CameraName, "isSwitchable", "()Z"));
    }

    bool Camera_name::is_unknown() const {
        attach_thread
        return env->CallBooleanMethod(cameraName, env->GetMethodID(CameraName, "isUnknown", "()Z"));
    }
} // sdk