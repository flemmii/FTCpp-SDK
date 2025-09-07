//
// Created by Frosch PC - V2 on 04.06.2024.
//

#include "tests/Driving.hpp"

using namespace sdk;
using namespace std;
using namespace gamepads;
using namespace linear_op_mode;

extern "C" void DrivingCpp() {
    // This gets all the drive motors from the hardware map
    Dc_motor_ex front_right = hardware_map::get<Dc_motor_ex>("front_right");
    Dc_motor_ex front_left = hardware_map::get<Dc_motor_ex>("front_left");
    Dc_motor_ex rear_right = hardware_map::get<Dc_motor_ex>("rear_right");
    Dc_motor_ex rear_left = hardware_map::get<Dc_motor_ex>("rear_left");

    // Reverses two motors, so forward is all motors to positive power
    // You may need to adjust this to meet your robots configuration
    front_left.set_direction(Dc_motor_ex::Direction::REVERSE);
    rear_left.set_direction(Dc_motor_ex::Direction::REVERSE);

    chrono::high_resolution_clock::time_point last_time = chrono::high_resolution_clock::now();
    chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();
    int loop_time;
    int count = 0;
    int min_loop_time = 1000000000;
    int max_loop_time = 0;
    double average_loop_time = 0;

    telemetry::add_line("Initialized");
    telemetry::update();

    wait_for_start();

    start_time = chrono::high_resolution_clock::now();
    last_time = chrono::high_resolution_clock::now();

    while (op_mode_is_active() && chrono::duration_cast<chrono::milliseconds>(
            chrono::high_resolution_clock::now() - start_time).count() < 60000) {
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

        average_loop_time = (average_loop_time * count + loop_time) / (count + 1);
        count++;

        if (loop_time > max_loop_time)
            max_loop_time = loop_time;

        if (loop_time < min_loop_time)
            min_loop_time = loop_time;

        telemetry::add_data("Loop time", loop_time);
        telemetry::add_data("Loop count", count);
        telemetry::add_data("Average loop time", average_loop_time);
        telemetry::add_data("Max loop time", max_loop_time);
        telemetry::add_data("Min loop time", min_loop_time);
        telemetry::update();

        last_time = chrono::high_resolution_clock::now();
    }

    while (!is_stop_requested());

    stop();
}