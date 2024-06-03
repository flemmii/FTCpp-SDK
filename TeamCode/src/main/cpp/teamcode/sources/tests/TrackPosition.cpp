//
// Created by fnlg on 03.06.2024.
//

#include "tests/TrackPosition.h"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_tests_cpp_TrackPositionCpp_opMode(JNIEnv *env,
                                                                      jobject thiz) {
    init_sdk

    sdk::Dc_motor_ex dead_wheel_forwards = hardware_map::get(DcMotorEx, "rear_left");
    sdk::Dc_motor_ex dead_wheel_sidewards = hardware_map::get(DcMotorEx, "front_right");

    dead_wheel_forwards.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
    dead_wheel_sidewards.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);

    dead_wheel_forwards.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
    dead_wheel_sidewards.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);

    Lynx_module control_hub = hardware_map::get(LynxModule, "Control Hub");
    Lynx_module expansion_hub = hardware_map::get(LynxModule, "Expansion Hub 2");
    control_hub.set_bulk_caching_mode(Lynx_module::MANUAL);
    expansion_hub.set_bulk_caching_mode(Lynx_module::MANUAL);

    class BNO055IMU imu = hardware_map::get(BNO055IMU, "imu1");
    BNO055IMU::Parameters parameters;
    parameters.angle_unit = BNO055IMU::Angle_unit::DEGREES;
    parameters.accel_unit = BNO055IMU::Accel_unit::METERS_PERSEC_PERSEC;
    imu.initialize(parameters);
    imu.start_acceleration_integration(100);

    int encoder_forward_pos = 0;
    int encoder_sidewards_pos = 0;

    double y_position = 0;
    double x_position = 0;
    double rotation_rad = 0;
    double rotation_deg = 0;
    double rotation_deg_continuous = 0;
    double extra_rad = 0;
    int extra_degrees = 0;

    chrono::high_resolution_clock::time_point last_time_millis = chrono::high_resolution_clock::now();
    int loop_time;
    vector<int> loop_times;
    int min_loop_time = 1000;
    int max_loop_time = 0;
    int avarage_loop_time;

    vector<double> ring_buffer_x_speed(5, 0);
    vector<double> ring_buffer_y_speed(5, 0);

    double y_speed;
    double x_speed;

    wait_for_start();

    last_time_millis = chrono::high_resolution_clock::now();

    while (!is_stop_requested()) {
        // Getting all the data of the hubs
        control_hub.get_bulk_data();
        if (control_hub.is_not_responding())
            logcat_log(ANDROID_LOG_FATAL, "track_position", "control hub not responding");

        expansion_hub.get_bulk_data();
        if (expansion_hub.is_not_responding())
            logcat_log(ANDROID_LOG_FATAL, "track_position", "expansion hub not responding");

        // Writing all the necessary logged data from the loop before into variables
        double previous_rotation_deg = rotation_deg;
        double previous_rotation_deg_continuous = rotation_deg_continuous;
        int previous_encoder_forward_pos = encoder_forward_pos;
        int previous_encoder_sidewards_pos = encoder_sidewards_pos;

        // Getting all the new data for tracking
        rotation_rad = -imu.get_angular_orientation().third_angle + extra_rad;
        encoder_forward_pos = -dead_wheel_forwards.get_current_position();
        encoder_sidewards_pos = dead_wheel_sidewards.get_current_position();
        double delta_t = (double) (chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - last_time_millis).count()) / 1000;

        // Converting radians to degrees and continuous degree system
        rotation_deg = (rotation_rad * 180) / M_PI;
        if (previous_rotation_deg - rotation_deg > 180)
            extra_degrees += 360;
        else if (previous_rotation_deg - rotation_deg < -180)
            extra_degrees -= 360;
        rotation_deg_continuous = rotation_deg + extra_degrees;

        // Calculating the delta values relative to the robots facing
        double delta_forward =
                ((double) (previous_encoder_forward_pos - encoder_forward_pos) /
                 ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM * M_PI;
        double delta_sidewards =
                ((double) (previous_encoder_sidewards_pos - encoder_sidewards_pos) /
                 ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM * M_PI;
        double delta_rotation =
                (previous_rotation_deg_continuous - rotation_deg_continuous) * (M_PI / 180);

        // This calculates how the deltas are if we got high rotation speed
        // Because if we got high rotation speed we'll rather drive a curve than a line
        if (fabs(delta_rotation) > 3) {
            double radius_forward = delta_forward / delta_rotation;
            double radius_sidewards = delta_sidewards / delta_rotation;

            double sinus_delta_rot = sin(delta_rotation);
            double cosinus_delta_rot = cos(delta_rotation);

            delta_forward = radius_forward * sinus_delta_rot +
                            radius_sidewards * (1 - cosinus_delta_rot);
            delta_sidewards = radius_forward * (1 - cosinus_delta_rot) -
                              radius_sidewards * sinus_delta_rot;
        }

        // Calculating the way we drove in x and y direction since the last loop
        double sinus = sin(rotation_rad);
        double cosinus = cos(rotation_rad);
        double delta_y = delta_forward * cosinus + delta_sidewards * sinus;
        double delta_x = delta_forward * sinus - delta_sidewards * cosinus;

        // Adding this way to the current position -> new position
        y_position += delta_y;
        x_position += delta_x;

        // Converting the deltas into the speeds
        delta_y /= delta_t;
        delta_x /= delta_t;

        // Stores the last n speeds in a ring buffer (n is defined at the declaration of the ring buffers)
        ring_buffer_x_speed.insert(ring_buffer_x_speed.begin(), delta_x);
        ring_buffer_y_speed.insert(ring_buffer_y_speed.begin(), delta_y);

        // Deletes the last element
        ring_buffer_x_speed.pop_back();
        ring_buffer_y_speed.pop_back();

        // Calculates the speeds through adding up all the last n calculated speeds
        // and then dividing them through n. This way the speeds get normalized and
        // we don't get completely wrong values
        y_speed = accumulate(ring_buffer_y_speed.begin(), ring_buffer_y_speed.end(), 0.0) /
                  ring_buffer_y_speed.size();
        x_speed = accumulate(ring_buffer_x_speed.begin(), ring_buffer_x_speed.end(), 0.0) /
                  ring_buffer_x_speed.size();

        // Calculating loop times
        loop_time = chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - last_time_millis).count();
        last_time_millis = chrono::high_resolution_clock::now();

        loop_times.push_back(loop_time);
        avarage_loop_time = accumulate(loop_times.begin(), loop_times.end(), 0) / loop_times.size();

        if (loop_time > max_loop_time)
            max_loop_time = loop_time;

        if (loop_time < min_loop_time)
            min_loop_time = loop_time;

        telemetry::add_data("Loop time", loop_time);
        telemetry::add_data("Avarage loop time", avarage_loop_time);
        telemetry::add_data("Max loop time", max_loop_time);
        telemetry::add_data("Min loop time", min_loop_time);
        telemetry::update();
    }

    stop();
}