//
// Created by Frosch PC - V2 on 05.02.2024.
//

#ifndef FTCROBOTCONTROLLER_CAMERA_NAME_H
#define FTCROBOTCONTROLLER_CAMERA_NAME_H

#include "extras/utils.h"

namespace sdk {
    extern jclass CameraName;

    class Camera_name {
    public:
        jobject cameraName{};

        Camera_name() = default;

        Camera_name(jobject cameraName);

        ~Camera_name();

        [[nodiscard]] bool is_webcam() const;

        [[nodiscard]] bool is_camera_direction() const;

        [[nodiscard]] bool is_switchable() const;

        [[nodiscard]] bool is_unknown() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_CAMERA_NAME_H
