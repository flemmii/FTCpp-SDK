//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_HARDWARE_MAP_H
#define FTCROBOTCONTROLLER_HARDWARE_MAP_H

#include <string>

#include "extras/utils.h"


namespace sdk::hardware_map {
    extern jobject hardwareMap;

    jobject get(jclass jclazz, const std::string &device_name);
}
// SDK

#endif //FTCROBOTCONTROLLER_HARDWARE_MAP_H
