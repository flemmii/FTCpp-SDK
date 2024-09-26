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

        BNO055IMU(const jobject &bno055imu);

        ~BNO055IMU();

        BNO055IMU &operator=(const jobject &bno055imu);

        enum class Angle_unit {
            DEGREES,
            RADIANS
        };

        static Angle_unit to_angle_unit(const Angle_unit &angle_unit) {
            return (angle_unit == Angle_unit::DEGREES) ? Angle_unit::DEGREES : Angle_unit::RADIANS;
        }

        static Angle_unit from_angle_unit(const Angle_unit &angle_unit) {
            return (angle_unit == Angle_unit::DEGREES) ? Angle_unit::DEGREES : Angle_unit::RADIANS;
        }

        static const char *angle_unit_to_string(const BNO055IMU::Angle_unit &angle_unit);

        enum class Accel_unit {
            METERS_PERSEC_PERSEC,
            MILLI_EARTH_GRAVITY
        };

        static const char *accel_unit_to_string(const Accel_unit &accel_unit);

        struct Parameters {
            static jclass jclazz;

            Angle_unit angle_unit = Angle_unit::RADIANS;
            Accel_unit accel_unit = Accel_unit::METERS_PERSEC_PERSEC;
        };

        bool initialize(const Parameters &parameters) const;

        // There are more parameters in Java but I am to lazy to implement them
        void start_acceleration_integration(const int &ms_poll_interval) const;

        [[nodiscard]] Orientation get_angular_orientation() const;

        [[nodiscard]] Acceleration get_acceleration() const;

        [[nodiscard]] Angular_velocity get_angular_velocity() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_BNO055IMU_H
