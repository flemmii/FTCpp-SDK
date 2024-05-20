//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_IMU_ORIENTATION_ON_ROBOT_H
#define FTCROBOTCONTROLLER_IMU_ORIENTATION_ON_ROBOT_H

#include "navigation/Quaternion.h"

namespace sdk {

    struct Imu_orientation_on_robot {
        Quaternion imuCoordinateSystemOrientationFromPerspectiveOfRobot();

        Quaternion imuRotationOffset();

        Quaternion angularVelocityTransform();
    };

} // sdk

#endif //FTCROBOTCONTROLLER_IMU_ORIENTATION_ON_ROBOT_H
