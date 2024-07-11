//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_HARDWARE_MAP_H
#define FTCROBOTCONTROLLER_HARDWARE_MAP_H

#include <string>

#include "utils.h"


namespace sdk::hardware_map {
    // TODO: This is not complete
    extern jobject hardwareMap;

    template<class T>
    jobject get(const std::string &device_name);
}
// SDK

#endif //FTCROBOTCONTROLLER_HARDWARE_MAP_H
