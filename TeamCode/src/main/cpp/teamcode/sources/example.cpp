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
    Dc_motor_ex back_right = hardware_map::get(DcMotorEx, "back_right");
    Dc_motor_ex back_left = hardware_map::get(DcMotorEx, "back_left");

    // The y direction of the stick is reversed in this sdk
    gamepad1.both_sticks.add_update_listener(
            [front_right, front_left, back_right, back_left](float left_stick_x, float left_stick_y,
                                                             float right_stick_x,
                                                             float right_stick_y) {
                // This sets the power of the motors to the y value of the left stick
                front_right.set_power(left_stick_y - left_stick_x - right_stick_x);
                front_left.set_power(left_stick_y + left_stick_x + right_stick_x);
                back_right.set_power(left_stick_y + left_stick_x - right_stick_x);
                back_left.set_power(left_stick_y - left_stick_x + right_stick_x);
            });

    telemetry::add_line("Initialized");
    telemetry::update();

    wait_for_start();

    while (op_mode_is_active()) {
        // This updates the gamepads and executes the listener defined above if needed
        gamepads::update();
    }
}