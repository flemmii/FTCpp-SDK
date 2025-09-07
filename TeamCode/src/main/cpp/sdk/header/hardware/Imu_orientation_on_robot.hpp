#pragma once

#include "navigation/Quaternion.hpp"

namespace sdk
{

    struct Imu_orientation_on_robot
    {
        Quaternion imuCoordinateSystemOrientationFromPerspectiveOfRobot();

        Quaternion imuRotationOffset();

        Quaternion angularVelocityTransform();
    };

} // sdk
