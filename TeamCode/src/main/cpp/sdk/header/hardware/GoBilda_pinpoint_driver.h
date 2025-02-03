//
// Created by flemmi on 03.02.25.
//

#ifndef FTCROBOTCONTROLLER_MODERN_ROBOTICS_COLOR_BEACON_H
#define FTCROBOTCONTROLLER_MODERN_ROBOTICS_COLOR_BEACON_H

#include <string>

#include "utils.h"

namespace sdk {
    class GoBilda_pinpoint_driver {
    public:
        static jclass jclazz;
        static jclass jclazz_DeviceStatus;
        static jclass jclazz_readData;
        static jclass jclazz_EncoderDirection;
        static jclass jclazz_GoBildaOdometryPods;
        jobject goBildaPinpointDriver{};

        GoBilda_pinpoint_driver() = default;

        GoBilda_pinpoint_driver(const jobject &goBildaPinpointDriver);

        ~GoBilda_pinpoint_driver();

        GoBilda_pinpoint_driver &
        operator=(const GoBilda_pinpoint_driver &goBildaPinpointDriver);

        GoBilda_pinpoint_driver &operator=(const jobject &goBildaPinpointDriver);

        bool do_initialize() const;

        [[nodiscard]] std::string get_device_name() const;

        enum class Device_status {
            NOT_READY = 0,
            READY = 1,
            CALIBRATING = 1 << 1,
            FAULT_X_POD_NOT_DETECTED = 1 << 2,
            FAULT_Y_POD_NOT_DETECTED = 1 << 3,
            FAULT_NO_PODS_DETECTED = (1 << 2) | (1 << 3),
            FAULT_IMU_RUNAWAY = 1 << 4
        };

        constexpr static char* device_status_to_string(Device_status status) {
            switch (status) {
                case Device_status::NOT_READY:
                    return "NOT_READY";
                case Device_status::READY:
                    return "READY";
                case Device_status::CALIBRATING:
                    return "CALIBRATING";
                case Device_status::FAULT_X_POD_NOT_DETECTED:
                    return "FAULT_X_POD_NOT_DETECTED";
                case Device_status::FAULT_Y_POD_NOT_DETECTED:
                    return "FAULT_Y_POD_NOT_DETECTED";
                case Device_status::FAULT_NO_PODS_DETECTED:
                    return "FAULT_NO_PODS_DETECTED";
                case Device_status::FAULT_IMU_RUNAWAY:
                    return "FAULT_IMU_RUNAWAY";
                default:
                    return "UNKNOWN";
            }
        }

        //enum that captures the direction the encoders are set to
        enum class Encoder_direction{
            FORWARD,
            REVERSED;
        };

        constexpr static char* encoder_direction_to_string(Encoder_direction direction) {
            switch (direction) {
                case Encoder_direction::FORWARD:
                    return "FORWARD";
                case Encoder_direction::REVERSED:
                    return "REVERSED";
                default:
                    return "UNKNOWN";
            }
        }

        //enum that captures the kind of goBILDA odometry pods, if goBILDA pods are used
        enum class GoBilda_odometry_pods {
            goBILDA_SWINGARM_POD,
            goBILDA_4_BAR_POD;
        };

        constexpr static char* goBilda_odometry_pods_to_string(GoBilda_odometry_pods pods) {
            switch (pods) {
                case GoBilda_odometry_pods::goBILDA_SWINGARM_POD:
                    return "goBILDA_SWINGARM_POD";
                case GoBilda_odometry_pods::goBILDA_4_BAR_POD:
                    return "goBILDA_4_BAR_POD";
                default:
                    return "UNKNOWN";
            }
        }

        //enum that captures a limited scope of read data. More options may be added in future update
        enum class read_data {
            ONLY_UPDATE_HEADING,
        };

        constexpr static char* read_data_to_string(read_data data) {
            switch (data) {
                case read_data::ONLY_UPDATE_HEADING:
                    return "ONLY_UPDATE_HEADING";
                default:
                    return "UNKNOWN";
            }
        }

        /**
         * Call this once per loop to read new data from the Odometry Computer. Data will only update once this is called.
         */
        void update();

        /**
         * Call this once per loop to read new data from the Odometry Computer. This is an override of the update() function
         * which allows a narrower range of data to be read from the device for faster read times. Currently ONLY_UPDATE_HEADING
         * is supported.
         * @param data GoBildaPinpointDriver.readData.ONLY_UPDATE_HEADING
         */
        void update(read_data data);

        /**
         * Sets the odometry pod positions relative to the point that the odometry computer tracks around.<br><br>
         * The most common tracking position is the center of the robot. <br> <br>
         * The X pod offset refers to how far sideways (in mm) from the tracking point the X (forward) odometry pod is. Left of the center is a positive number, right of center is a negative number. <br>
         * the Y pod offset refers to how far forwards (in mm) from the tracking point the Y (strafe) odometry pod is. forward of center is a positive number, backwards is a negative number.<br>
         * @param x_offset how sideways from the center of the robot is the X (forward) pod? Left increases
         * @param y_Offset how far forward from the center of the robot is the Y (Strafe) pod? forward increases
         */
        void set_offsets(double x_offset, double y_offset);

        /**
         * Recalibrates the Odometry Computer's internal IMU. <br><br>
         * <strong> Robot MUST be stationary </strong> <br><br>
         * Device takes a large number of samples, and uses those as the gyroscope zero-offset. This takes approximately 0.25 seconds.
         */
        void recalibrate_IMU();

        /**
         * Resets the current position to 0,0,0 and recalibrates the Odometry Computer's internal IMU. <br><br>
         * <strong> Robot MUST be stationary </strong> <br><br>
         * Device takes a large number of samples, and uses those as the gyroscope zero-offset. This takes approximately 0.25 seconds.
         */
        void reset_pos_and_IMU();

        /**
         * Can reverse the direction of each encoder.
         * @param x_encoder FORWARD or REVERSED, X (forward) pod should increase when the robot is moving forward
         * @param y_encoder FORWARD or REVERSED, Y (strafe) pod should increase when the robot is moving left
         */
        void set_encoder_directions(Encoder_direction x_encoder, Encoder_direction y_encoder);

        /**
         * If you're using goBILDA odometry pods, the ticks-per-mm values are stored here for easy access.<br><br>
         * @param pods goBILDA_SWINGARM_POD or goBILDA_4_BAR_POD
         */
        void set_encoder_resolution(GoBilda_ddometry_pods pods);

        /**
         * Sets the encoder resolution in ticks per mm of the odometry pods. <br>
         * You can find this number by dividing the counts-per-revolution of your encoder by the circumference of the wheel.
         * @param ticks_per_mm should be somewhere between 10 ticks/mm and 100 ticks/mm a goBILDA Swingarm pod is ~13.26291192
         */
        void set_encoder_resolution(double ticks_per_mm);

        /**
         * Tuning this value should be unnecessary.<br>
         * The goBILDA Odometry Computer has a per-device tuned yaw offset already applied when you receive it.<br><br>
         * This is a scalar that is applied to the gyro's yaw value. Increasing it will mean it will report more than one degree for every degree the sensor fusion algorithm measures. <br><br>
         * You can tune this variable by rotating the robot a large amount (10 full turns is a good starting place) and comparing the amount that the robot rotated to the amount measured.
         * Rotating the robot exactly 10 times should measure 3600°. If it measures more or less, divide moved amount by the measured amount and apply that value to the Yaw Offset.<br><br>
         * If you find that to get an accurate heading number you need to apply a scalar of more than 1.05, or less than 0.95, your device may be bad. Please reach out to tech@gobilda.com
         * @param yaw_offset A scalar for the robot's heading.
         */
        void set_yaw_scalar(double yaw_offset);

        /**
         * Send a position that the Pinpoint should use to track your robot relative to. You can use this to
         * update the estimated position of your robot with new external sensor data, or to run a robot
         * in field coordinates. <br><br>
         * This overrides the current position. <br><br>
         * <strong>Using this feature to track your robot's position in field coordinates:</strong> <br>
         * When you start your code, send a Pose2D that describes the starting position on the field of your robot. <br>
         * Say you're on the red alliance, your robot is against the wall and closer to the audience side,
         * and the front of your robot is pointing towards the center of the field.
         * You can send a setPosition with something like -600mm x, -1200mm Y, and 90 degrees. The pinpoint would then always
         * keep track of how far away from the center of the field you are. <br><br>
         * <strong>Using this feature to update your position with additional sensors: </strong><br>
         * Some robots have a secondary way to locate their robot on the field. This is commonly
         * Apriltag localization in FTC, but it can also be something like a distance sensor.
         * Often these external sensors are absolute (meaning they measure something about the field)
         * so their data is very accurate. But they can be slower to read, or you may need to be in a very specific
         * position on the field to use them. In that case, spend most of your time relying on the Pinpoint
         * to determine your location. Then when you pull a new position from your secondary sensor,
         * send a setPosition command with the new position. The Pinpoint will then track your movement
         * relative to that new, more accurate position.
         * @param pos a Pose2D describing the robot's new position.
         */
        Pose2D set_position(Pose2D pos);

        /**
         * Checks the deviceID of the Odometry Computer. Should return 1.
         * @return 1 if device is functional.
         */
        int get_device_ID();

        /**
         * @return the firmware version of the Odometry Computer
         */
        int get_device_version();

        float get_yaw_scalar();

        /**
         * Device Status stores any faults the Odometry Computer may be experiencing. These faults include:
         * @return one of the following states:<br>
         * NOT_READY - The device is currently powering up. And has not initialized yet. RED LED<br>
         * READY - The device is currently functioning as normal. GREEN LED<br>
         * CALIBRATING - The device is currently recalibrating the gyro. RED LED<br>
         * FAULT_NO_PODS_DETECTED - the device does not detect any pods plugged in. PURPLE LED <br>
         * FAULT_X_POD_NOT_DETECTED - The device does not detect an X pod plugged in. BLUE LED <br>
         * FAULT_Y_POD_NOT_DETECTED - The device does not detect a Y pod plugged in. ORANGE LED <br>
         */
        Device_status get_device_status();

        /**
         * Checks the Odometry Computer's most recent loop time.<br><br>
         * If values less than 500, or more than 1100 are commonly seen here, there may be something wrong with your device. Please reach out to tech@gobilda.com
         * @return loop time in microseconds (1/1,000,000 seconds)
         */
        int get_loop_time();

        /**
         * Checks the Odometry Computer's most recent loop frequency.<br><br>
         * If values less than 900, or more than 2000 are commonly seen here, there may be something wrong with your device. Please reach out to tech@gobilda.com
         * @return Pinpoint Frequency in Hz (loops per second),
         */
        double get_frequency();

        /**
         * @return the raw value of the X (forward) encoder in ticks
         */
        int get_encoder_x();

        /**
         * @return the raw value of the Y (strafe) encoder in ticks
         */
        int get_encoder_y();

        /**
         * @return the estimated X (forward) position of the robot in mm
         */
        double get_pos_x();

        /**
         * @return the estimated Y (Strafe) position of the robot in mm
         */
        double get_pos_y();

        /**
         * @return the estimated H (heading) position of the robot in Radians
         */
        double get_heading();

        /**
         * @return the estimated X (forward) velocity of the robot in mm/sec
         */
        double get_vel_x();

        /**
         * @return the estimated Y (strafe) velocity of the robot in mm/sec
         */
        double get_vel_y();

        /**
         * @return the estimated H (heading) velocity of the robot in radians/sec
         */
        double get_heading_velocity();

        /**
         * <strong> This uses its own I2C read, avoid calling this every loop. </strong>
         * @return the user-set offset for the X (forward) pod
         */
        float get_x_offset();

        /**
         * <strong> This uses its own I2C read, avoid calling this every loop. </strong>
         * @return the user-set offset for the Y (strafe) pod
         */
        float get_y_offset();

        // TODO: Implement the following functions
        /**
         * @return a Pose2D containing the estimated position of the robot
         */
        // Pose2D get_position();

        /**
         * @return a Pose2D containing the estimated velocity of the robot, velocity is unit per second
         */
        // Pose2D get_velocity();
    };
}

#endif //FTCROBOTCONTROLLER_MODERN_ROBOTICS_COLOR_BEACON_H
