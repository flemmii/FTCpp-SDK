//
// Created by Frosch PC - V2 on 05.02.2024.
//

#ifndef FTCROBOTCONTROLLER_WEBCAM_NAME_H
#define FTCROBOTCONTROLLER_WEBCAM_NAME_H

#include <string>

#include "extras/utils.h"

#include "Camera_name.h"

namespace sdk {
    extern jclass WebcamName;

    class Webcam_name : public Camera_name {
    public:
        jobject webcamName{};

        Webcam_name() = default;

        Webcam_name(jobject webcamName);

        ~Webcam_name();

        [[nodiscard]] std::string get_usb_device_name_if_attached() const;

        [[nodiscard]] bool is_attached() const;
    };
}


#endif //FTCROBOTCONTROLLER_WEBCAM_NAME_H
