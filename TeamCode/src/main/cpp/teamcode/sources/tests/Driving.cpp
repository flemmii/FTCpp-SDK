//
// Created by Frosch PC - V2 on 04.06.2024.
//

#include "tests/Driving.h"

using namespace sdk;
using namespace std;
using namespace gamepads;
using namespace linear_op_mode;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_cpp_tests_DrivingCpp_opMode(JNIEnv *env,
                                                                  jobject thiz) {
    // This initializes the SDK, it needs to be called at the start of every opmode
    init_sdk

    // This gets all the drive motors from the hardware map
    Dc_motor_ex front_right = hardware_map::get(DcMotorEx, "front_right");
    Dc_motor_ex front_left = hardware_map::get(DcMotorEx, "front_left");
    Dc_motor_ex rear_right = hardware_map::get(DcMotorEx, "rear_right");
    Dc_motor_ex rear_left = hardware_map::get(DcMotorEx, "rear_left");

    // Reverses two motors, so forward is all motors to positive power
    // You may need to adjust this to meet your robots configuration
    front_left.set_direction(Dc_motor_ex::Direction::REVERSE);
    rear_left.set_direction(Dc_motor_ex::Direction::REVERSE);

    chrono::high_resolution_clock::time_point last_time = chrono::high_resolution_clock::now();
    chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();
    int loop_time;
    vector<int> loop_times;
    int min_loop_time = 1000000000;
    int max_loop_time = 0;
    double avarage_loop_time;

    telemetry::add_line("Initialized");
    telemetry::update();

    wait_for_start();

    start_time = chrono::high_resolution_clock::now();
    last_time = chrono::high_resolution_clock::now();

    while (op_mode_is_active() && chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_time).count() < 60000) {
        // This updates the gamepads and needs to get called every loop
        gamepads::update();

        // The y direction of the stick is reversed in this sdk
        float left_stick_y = -gamepad1.left_stick_y;
        float left_stick_x = gamepad1.left_stick_x;
        float right_stick_x = gamepad1.right_stick_x;

        // This sets the power of the motors to the y value of the left stick
        front_right.set_power(left_stick_y - left_stick_x - right_stick_x);
        front_left.set_power(left_stick_y + left_stick_x + right_stick_x);
        rear_right.set_power(left_stick_y + left_stick_x - right_stick_x);
        rear_left.set_power(left_stick_y - left_stick_x + right_stick_x);

        // Calculating loop times
        loop_time = chrono::duration_cast<chrono::microseconds>(
                chrono::high_resolution_clock::now() - last_time).count();
        last_time = chrono::high_resolution_clock::now();

        loop_times.push_back(loop_time);
        avarage_loop_time = accumulate(loop_times.begin(), loop_times.end(), 0) / (double) (loop_times.size());

        if (loop_time > max_loop_time)
            max_loop_time = loop_time;

        if (loop_time < min_loop_time)
            min_loop_time = loop_time;

        telemetry::add_data("Loop time", loop_time);
        telemetry::add_data("Loop times size", (double) (loop_times.size()));
        telemetry::add_data("Avarage loop time", avarage_loop_time);
        telemetry::add_data("Max loop time", max_loop_time);
        telemetry::add_data("Min loop time", min_loop_time);
        telemetry::update();
    }

    while(!is_stop_requested());

    stop();
}