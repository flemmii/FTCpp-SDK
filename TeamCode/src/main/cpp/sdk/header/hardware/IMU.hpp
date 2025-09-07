#pragma once

#include "utils.hpp"

#include "navigation/Yaw_pitch_roll_angles.hpp"

namespace sdk
{
    // TODO: Finish this class
    /*
    class IMU {
    public:
        static jclass jclazz;
        jobject imu{};

        static struct Parameters {
            ImuOrientationOnRobot imuOrientationOnRobot;

            Parameters(ImuOrientationOnRobot imuOrientationOnRobot) {
                this.imuOrientationOnRobot = imuOrientationOnRobot;
            }

            Parameters copy() {
                return new Parameters(this.imuOrientationOnRobot);
            }
        };

        bool initialize(Parameters var1);

        void reset_yaw();

        Yaw_pitch_roll_angles get_robot_yaw_pitch_roll_angles();

        Orientation getRobotOrientation(AxesReference var1, AxesOrder var2, AngleUnit var3);

        Quaternion getRobotOrientationAsQuaternion();

        AngularVelocity getRobotAngularVelocity(AngleUnit var1);
    };
     */
} // sdk
