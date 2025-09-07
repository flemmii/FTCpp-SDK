#pragma once

#include "utils.hpp"

namespace sdk
{
    class Camera_name
    {
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
