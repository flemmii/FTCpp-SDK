//
// Created by fnlg on 12.05.2024.
//

#include "example.h"

using namespace sdk;
using namespace gamepads;
using namespace linear_op_mode;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_teleop_Example_opMode(JNIEnv *env,
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

    telemetry::add_line("Initialized");
    telemetry::update();

    wait_for_start();

    while (op_mode_is_active()) {
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
    }
}