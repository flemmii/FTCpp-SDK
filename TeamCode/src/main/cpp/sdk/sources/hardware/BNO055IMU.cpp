//
// Created by fnlg on 21.01.2024.
//

#include "hardware/BNO055IMU.h"

namespace sdk {
    jclass BNO055IMU;
    jclass Parameters;
    jclass AngleUnit;
    jclass AccelUnit;

    BNO055IMU::BNO055IMU(jobject bno055imu) : bno055imu(bno055imu) {}

    const char *BNO055IMU::angle_unit_to_string(BNO055IMU::Angle_unit angle_unit) {
        switch (angle_unit) {
            case BNO055IMU::Angle_unit::DEGREES:
                return "DEGREES";
            case BNO055IMU::Angle_unit::RADIANS:
                return "RADIANS";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    const char *BNO055IMU::accel_unit_to_string(Accel_unit accel_unit) {
        switch (accel_unit) {
            case BNO055IMU::Accel_unit::METERS_PERSEC_PERSEC:
                return "METERS_PERSEC_PERSEC";
            case BNO055IMU::Accel_unit::MILLI_EARTH_GRAVITY:
                return "MILLI_EARTH_GRAVITY";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    bool BNO055IMU::initialize(sdk::BNO055IMU::Parameters parameters) {
        attach_thread
        jobject parametersJava = env->NewObject(sdk::Parameters,
                                                env->GetMethodID(sdk::Parameters, "<init>", "()V"));
        jobject angleUnit = env->GetStaticObjectField(sdk::AngleUnit,
                                                      env->GetStaticFieldID(sdk::AngleUnit,
                                                                            angle_unit_to_string(
                                                                                    parameters.angle_unit),
                                                                            "Lcom/qualcomm/hardware/bosch/BNO055IMU$AngleUnit;"));
        jobject accelUnit = env->GetStaticObjectField(sdk::AccelUnit,
                                                      env->GetStaticFieldID(sdk::AccelUnit,
                                                                            accel_unit_to_string(
                                                                                    parameters.accel_unit),
                                                                            "Lcom/qualcomm/hardware/bosch/BNO055IMU$AccelUnit;"));
        env->SetObjectField(parametersJava, env->GetFieldID(sdk::Parameters, "angleUnit",
                                                            "Lcom/qualcomm/hardware/bosch/BNO055IMU$AngleUnit;"),
                            angleUnit);
        env->SetObjectField(parametersJava, env->GetFieldID(sdk::Parameters, "accelUnit",
                                                            "Lcom/qualcomm/hardware/bosch/BNO055IMU$AccelUnit;"),
                            accelUnit);

        bool result = env->CallBooleanMethod(bno055imu,
                                             env->GetMethodID(sdk::BNO055IMU, "initialize",
                                                              "(Lcom/qualcomm/hardware/bosch/BNO055IMU$Parameters;)Z"),
                                             parametersJava);
        env->DeleteLocalRef(parametersJava);
        env->DeleteLocalRef(angleUnit);
        env->DeleteLocalRef(accelUnit);

        return result;
    }

    void BNO055IMU::start_acceleration_integration(int ms_poll_interval) {
        attach_thread
        env->CallVoidMethod(bno055imu,
                            env->GetMethodID(sdk::BNO055IMU, "startAccelerationIntegration",
                                             "(Lorg/firstinspires/ftc/robotcore/external/navigation/Position;Lorg/firstinspires/ftc/robotcore/external/navigation/Velocity;I)V"),
                            NULL, NULL, static_cast<jint>(ms_poll_interval));
    }

    Orientation BNO055IMU::get_angular_orientation() {
        attach_thread
        jobject orientation = env->CallObjectMethod(bno055imu, env->GetMethodID(sdk::BNO055IMU,
                                                                                "getAngularOrientation",
                                                                                "()Lorg/firstinspires/ftc/robotcore/external/navigation/Orientation;"));
        jclass Orientation = env->GetObjectClass(orientation);
        auto x = static_cast<float>(env->GetFloatField(orientation,
                                                       env->GetFieldID(Orientation, "firstAngle",
                                                                       "F")));
        auto y = static_cast<float>(env->GetFloatField(orientation,
                                                       env->GetFieldID(Orientation, "secondAngle",
                                                                       "F")));
        auto z = static_cast<float>(env->GetFloatField(orientation,
                                                       env->GetFieldID(Orientation, "thirdAngle",
                                                                       "F")));

        env->DeleteLocalRef(orientation);
        env->DeleteLocalRef(Orientation);
        return {x, y, z};
    }

    Acceleration BNO055IMU::get_acceleration() {
        attach_thread
        jobject acceleration = env->CallObjectMethod(bno055imu, env->GetMethodID(sdk::BNO055IMU,
                                                                                 "getAcceleration",
                                                                                 "()Lorg/firstinspires/ftc/robotcore/external/navigation/Acceleration;"));
        jclass Acceleration = env->GetObjectClass(acceleration);
        auto x = static_cast<float>(env->GetFloatField(acceleration,
                                                       env->GetFieldID(Acceleration, "xAccel",
                                                                       "F")));
        auto y = static_cast<float>(env->GetFloatField(acceleration,
                                                       env->GetFieldID(Acceleration, "yAccel",
                                                                       "F")));
        auto z = static_cast<float>(env->GetFloatField(acceleration,
                                                       env->GetFieldID(Acceleration, "zAccel",
                                                                       "F")));

        env->DeleteLocalRef(acceleration);
        env->DeleteLocalRef(Acceleration);
        return {x, y, z};
    }

    Angular_velocity BNO055IMU::get_angular_velocity() {
        attach_thread
        jobject angularVelocity = env->CallObjectMethod(bno055imu, env->GetMethodID(sdk::BNO055IMU,
                                                                                    "getAngularVelocity",
                                                                                    "()Lorg/firstinspires/ftc/robotcore/external/navigation/AngularVelocity;"));
        jclass AngularVelocity = env->GetObjectClass(angularVelocity);
        auto x = static_cast<float>(env->GetFloatField(angularVelocity,
                                                       env->GetFieldID(AngularVelocity,
                                                                       "xRotationRate",
                                                                       "F")));
        auto y = static_cast<float>(env->GetFloatField(angularVelocity,
                                                       env->GetFieldID(AngularVelocity,
                                                                       "yRotationRate",
                                                                       "F")));
        auto z = static_cast<float>(env->GetFloatField(angularVelocity,
                                                       env->GetFieldID(AngularVelocity,
                                                                       "zRotationRate",
                                                                       "F")));

        env->DeleteLocalRef(angularVelocity);
        env->DeleteLocalRef(AngularVelocity);
        return {x, y, z};
    }
} // sdk