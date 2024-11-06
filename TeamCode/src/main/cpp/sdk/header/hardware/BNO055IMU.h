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

        BNO055IMU &operator=(const BNO055IMU &bno055imu);

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

        /**
         * Returns the absolute orientation of the sensor as a set three angles with indicated parameters.
         * @param reference the axes reference in which the result will be expressed
         * @param order     the axes order in which the result will be expressed
         * @param angleUnit the angle units in which the result will be expressed
         * @return the absolute orientation of the sensor
         * @see Orientation
         * @see #getAngularOrientation()
         */
        [[nodiscard]] Orientation get_angular_orientation() const;

        /**
         * Returns the overall acceleration experienced by the sensor. This is composed of
         * a component due to the movement of the sensor and a component due to the force of gravity.
         * @return  the overall acceleration vector experienced by the sensor
         * @see #getLinearAcceleration()
         * @see #getGravity()
         */
        [[nodiscard]] Acceleration get_overall_acceleration() const;

        /**
         * Returns the rate of change of the absolute orientation of the sensor.
         * @return the rate at which the orientation of the sensor is changing.
         * @see #getAngularOrientation()
         */
        [[nodiscard]] Angular_velocity get_angular_velocity() const;

        /**
         * Returns the last observed acceleration of the sensor. Note that this does not communicate
         * with the sensor, but rather returns the most recent value reported to the acceleration
         * integration algorithm.
         * @return  the last observed acceleration of the sensor
         * @see #getLinearAcceleration()
         * @see Parameters#accelerationIntegrationAlgorithm
         * @see #startAccelerationIntegration(Position, Velocity, int)
         */
        [[nodiscard]] Acceleration get_linear_acceleration() const;

        /**
         * Returns the direction of the force of gravity relative to the sensor.
         * @return  the acceleration vector of gravity relative to the sensor
         * @see #getOverallAcceleration()
         * @see #getLinearAcceleration()
         */
        [[nodiscard]] Acceleration get_gravity() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_BNO055IMU_H
