//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_ANGLE_UNIT_H
#define FTCROBOTCONTROLLER_ANGLE_UNIT_H

#include <jni.h>

namespace sdk {
    extern jclass AngleUnit;
    enum class Angle_unit {
        // TODO: This is not complete
        DEGREES,
        RADIANS
    };
} // sdk


#endif //FTCROBOTCONTROLLER_ANGLE_UNIT_H
