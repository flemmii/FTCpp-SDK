//
// Created by fnlg on 21.01.2024.
//

#ifndef FTCROBOTCONTROLLER_BNO055IMU_H
#define FTCROBOTCONTROLLER_BNO055IMU_H

#include "extras/utils.h"
#include "extras/Vec3d.h"

namespace sdk {
    extern jclass BNO055IMU;
    extern jclass Parameters;
    extern jclass AngleUnit;
    extern jclass AccelUnit;

    class BNO055IMU {
        jobject bno055imu;
        Vec3d::Vec3d_remap_order axis_remap_order;
    public:


        enum class Angle_unit {
            DEGREES,
            RADIANS
        };

        static Angle_unit to_angle_unit(Angle_unit angle_unit) {
            return (angle_unit == Angle_unit::DEGREES) ? Angle_unit::DEGREES : Angle_unit::RADIANS;
        }

        static Angle_unit from_angle_unit(Angle_unit angle_unit) {
            return (angle_unit == Angle_unit::DEGREES) ? Angle_unit::DEGREES : Angle_unit::RADIANS;
        }

        static const char *angle_unit_to_string(BNO055IMU::Angle_unit angle_unit);

        enum class Accel_unit {
            METERS_PERSEC_PERSEC,
            MILLI_EARTH_GRAVITY
        };

        static const char *accel_unit_to_string(Accel_unit accel_unit);

        class Parameters {
        public:
            Angle_unit angle_unit = Angle_unit::RADIANS;
            Accel_unit accel_unit = Accel_unit::METERS_PERSEC_PERSEC;
        };

        bool initialize(Parameters parameters);

        // There are more parameters in Java but I am to lazy to implement them
        void start_acceleration_integration(int ms_poll_interval);

        Vec3d get_rotation();

        Vec3d get_acceleration();

        Vec3d get_angular_velocity();

        static sdk::BNO055IMU::Parameters defaultParameters();

        BNO055IMU(jobject imu, BNO055IMU::Parameters parameters,
                  Vec3d::Vec3d_remap_order axis_remap_order);

        BNO055IMU(jobject imu, Vec3d::Vec3d_remap_order axis_remap_order);

        BNO055IMU(jobject imu);

        BNO055IMU() = default;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_BNO055IMU_H
