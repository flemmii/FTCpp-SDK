//
// Created by Frosch PC - V2 on 05.02.2024.
//

#include "../../../header/opmodes/tele_op/MoveSuspensionArm.h"

using namespace std;
using namespace sdk;
using namespace linear_op_mode;
using namespace gamepads;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_MoveSuspensionArm_opMode(JNIEnv *env,
                                                                                      jobject thiz) {
    init_sdk
    robot::init();

    suspension_arm_and_drone_launcher::use_encoder(false);
    gamepad1.left_stick.add_update_listener([](float x, float y) {
        suspension_arm_and_drone_launcher::set_target_power(y);
    });
    gamepad1.a.add_button_press_listener(
            []() { suspension_arm_and_drone_launcher::launch_drone(); });
    gamepad1.b.add_button_press_listener([]() { suspension_arm_and_drone_launcher::test(); });
    gamepad1.right_trigger.add_press_listener([](float power) { elevator::set_power(-power); });
    gamepad1.x.add_button_press_listener([]() { elevator::set_power(0.5); });
    gamepad1.y.add_button_press_listener([]() { output::release_pixel(); });
    telemetry::add_line("initialized");
    telemetry::update();
    wait_for_start();

    auto start_time = chrono::high_resolution_clock::now();
    while (op_mode_is_active()) {
        telemetry::add_data("degrees",
                            suspension_arm_and_drone_launcher::get_elevation_in_degrees());
        gamepads::update();
    }

    stop();
}