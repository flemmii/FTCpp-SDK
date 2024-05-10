//
// Created by Frosch PC - V2 on 01.02.2024.
//

#include "../../../header/opmodes/tele_op/Guest1Driver.h"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace gamepads;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_Guest1Driver_opMode(JNIEnv *env,
                                                                                 jobject thiz) {
    init_sdk

    robot::init();
    track_position::init();
    drivetrain::init();
    intake::init();

    drivetrain::preset_for_tele_op();

    gamepad1.right_bumper.add_button_press_listener([]() { intake::activate_intake(); });
    gamepad1.right_bumper.add_button_release_listener([]() { intake::deactivate_intake(); });

    gamepad1.left_bumper.add_button_press_listener([]() { intake::activate_intake_reversed(); });
    gamepad1.left_bumper.add_button_release_listener([]() { intake::deactivate_intake(); });

    gamepad1.both_sticks.add_update_listener(
            [](float left_stick_x, float left_stick_y, float right_stick_x,
               float right_stick_y) {
                drivetrain::drive(left_stick_y, left_stick_x, right_stick_x, 0.5);
            });

    telemetry::add_line("initialized");
    telemetry::update();
    wait_for_start();

    auto start_time = chrono::high_resolution_clock::now();
    while (op_mode_is_active()) {

        gamepads::update();
        drivetrain::do_drive();
    }

    stop();
}