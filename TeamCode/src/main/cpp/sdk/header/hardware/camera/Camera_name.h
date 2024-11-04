//
// Created by Frosch PC - V2 on 05.02.2024.
//

#ifndef FTCROBOTCONTROLLER_CAMERA_NAME_H
#define FTCROBOTCONTROLLER_CAMERA_NAME_H

#include "utils.h"

namespace sdk {
    class Camera_name {
    public:
        static jclass jclazz;
        jobject cameraName{};

        Camera_name() = default;

        Camera_name(const jobject &cameraName);

        ~Camera_name();

        Camera_name &operator=(const Camera_name &camera_name);

        virtual Camera_name &operator=(const jobject &cameraName);

        [[nodiscard]] bool is_webcam() const;

        [[nodiscard]] bool is_camera_direction() const;

        [[nodiscard]] bool is_switchable() const;

        [[nodiscard]] bool is_unknown() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_CAMERA_NAME_H
