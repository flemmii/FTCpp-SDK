//
// Created by fnlg on 21.01.2024.
//

#ifndef FTCROBOTCONTROLLER_BNO055IMU_H
#define FTCROBOTCONTROLLER_BNO055IMU_H

#include "utils.h"

#include "navigation/Orientation.h"
#include "navigation/Acceleration.h"
#include "navigation/Angular_velocity.h"

namespace sdk {
    class BNO055IMU {
        //TODO: This is not complete
    public:
        static jclass jclazz;
        static jclass jclazz_AngleUnit;
        static jclass jclazz_AccelUnit;
        jobject bno055imu{};

        BNO055IMU() = default;

        BNO055IMU(jobject bno055imu);

        ~BNO055IMU();

        BNO055IMU &operator=(jobject bno055imu);

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

        struct Parameters {
            static jclass jclazz;

            Angle_unit angle_unit = Angle_unit::RADIANS;
            Accel_unit accel_unit = Accel_unit::METERS_PERSEC_PERSEC;
        };

        bool initialize(Parameters parameters);

        // There are more parameters in Java but I am to lazy to implement them
        void start_acceleration_integration(int ms_poll_interval);

        [[nodiscard]] Orientation get_angular_orientation();

        [[nodiscard]] Acceleration get_acceleration();

        [[nodiscard]] Angular_velocity get_angular_velocity();
    };

} // sdk

#endif //FTCROBOTCONTROLLER_BNO055IMU_H
