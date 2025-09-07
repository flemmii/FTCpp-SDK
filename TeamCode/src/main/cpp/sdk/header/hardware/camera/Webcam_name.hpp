#pragma once

#include <string>

#include "utils.hpp"

#include "Camera_name.hpp"

namespace sdk
{
    class Webcam_name : public Camera_name
    {
    public:
        static jclass jclazz;
        jobject webcamName{};

        Webcam_name() = default;

        Webcam_name(const jobject &webcamName);

        ~Webcam_name();

        Webcam_name &operator=(const Webcam_name &webcam_name);

        Webcam_name &operator=(const jobject &webcamName) override;

        [[nodiscard]] std::string get_usb_device_name_if_attached() const;

        [[nodiscard]] bool is_attached() const;
    };
}
