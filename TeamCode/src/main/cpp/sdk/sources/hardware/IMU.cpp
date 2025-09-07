//
// Created by fnlg on 20.05.2024.
//

#include "hardware/IMU.hpp"

namespace sdk
{
        /*
        bool IMU::initialize(Parameters parameters) {

        }

        void IMU::reset_yaw() {
            attach_thread
            env->CallVoidMethod(imu, env->GetMethodID(jclazz, "resetYaw", "()V"));
        }

        Yaw_pitch_roll_angles IMU::get_robot_yaw_pitch_roll_angles() {
            attach_thread
            jobject yawPitchRollAngles = env->CallObjectMethod(imu, env->GetMethodID(jclazz,
                                                                                     "getRobotYawPitchRollAngles",
                                                                                     "()Lorg/firstinspires/ftc/robotcore/external/navigation/YawPitchRoll;"));
            jclass YawPitchRollAngles = env->GetObjectClass(yawPitchRollAngles);
            jobject DEGREES = env->GetStaticObjectField(AngleUnit,
                                                        env->GetStaticFieldID(AngleUnit,
                                                                              "DEGREES",
                                                                              "Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;"));

            double yaw_degrees = static_cast<double>(env->CallDoubleMethod(yawPitchRollAngles,
                                                                           env->GetMethodID(
                                                                                   YawPitchRollAngles,
                                                                                   "getYaw",
                                                                                   "(Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;)D"),
                                                                           DEGREES));
            double pitch_degrees = static_cast<double>(env->CallDoubleMethod(yawPitchRollAngles,
                                                                             env->GetMethodID(
                                                                                     YawPitchRollAngles,
                                                                                     "getPitch",
                                                                                     "(Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;)D"),
                                                                             DEGREES));
            double roll_degrees = static_cast<double>(env->CallDoubleMethod(yawPitchRollAngles,
                                                                            env->GetMethodID(
                                                                                    YawPitchRollAngles,
                                                                                    "getRoll",
                                                                                    "(Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;)D"),
                                                                            DEGREES));
            long acquisition_time = static_cast<long>(env->CallLongMethod(yawPitchRollAngles,
                                                                          env->GetMethodID(
                                                                                  YawPitchRollAngles,
                                                                                  "acquisitionTime",
                                                                                  "()J")));

            env->DeleteLocalRef(yawPitchRollAngles);
            env->DeleteLocalRef(YawPitchRollAngles);
            env->DeleteLocalRef(DEGREES);
            return {Angle_unit::DEGREES, yaw_degrees, pitch_degrees, roll_degrees, acquisition_time};
        }
         */
} // sdk