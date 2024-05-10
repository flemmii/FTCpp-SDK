//
// Created by fnlg on 28.12.2023.
//

#include "../../../header/opmodes/tele_op/Main.h"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace gamepads;

void switch_headless_off_if_all_buttons_pressed(int buttons_pressed_for_switching_headless_on_off) {
    if (buttons_pressed_for_switching_headless_on_off == 4) {
        drivetrain::use_headless = !drivetrain::use_headless;
    }
}

bool drive_fast = false;
bool execute_do_drive = false;
int buttons_pressed_for_switching_headless_on_off = 0;
bool drive_to_position_mode = false;
bool elevate_arm = false;
bool use_gyro_support = true;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_CppTest_opMode(JNIEnv *env,
                                                                            jobject thiz) {
    init_sdk
    robot::init();

    //drive_to_position_controller::set_max_deviation(100, 100, 20);
    drive_to_position_controller::set_max_motor_power(1);
    suspension_arm_and_drone_launcher::use_encoder(true);
    last_position_storage::init(false);

    if (last_position_storage::valid_data_available()) {
        track_position::set_position(last_position_storage::get_last_position());
    }

    // A-DRIVER
    gamepad1.both_sticks.add_update_listener(
            [](float left_stick_x, float left_stick_y,
               float right_stick_x,
               float right_stick_y) {
                if (!drive_to_position_mode) {
                    if (drive_fast) {
                        drivetrain::drive(left_stick_y, left_stick_x, right_stick_x);
                    } else {
                        drivetrain::drive(left_stick_y, left_stick_x, right_stick_x, 0.4);
                    }
                    execute_do_drive = true;
                }
            });
    gamepad1.left_stick_button.add_button_press_listener([]() { track_position::set_rotation(0); });

    gamepad1.left_trigger.add_full_press_listener(
            [](float value) {
                buttons_pressed_for_switching_headless_on_off++;
                switch_headless_off_if_all_buttons_pressed(
                        buttons_pressed_for_switching_headless_on_off);
            });
    gamepad1.left_trigger.add_trigger_release_listener(
            [](
                    float value) { buttons_pressed_for_switching_headless_on_off--; });

    gamepad1.right_trigger.add_full_press_listener(
            [](float value) {
                buttons_pressed_for_switching_headless_on_off++;
                switch_headless_off_if_all_buttons_pressed(
                        buttons_pressed_for_switching_headless_on_off);
            });
    gamepad1.right_trigger.add_trigger_release_listener(
            [](
                    float value) { buttons_pressed_for_switching_headless_on_off--; });

    gamepad1.left_bumper.add_button_press_listener(
            []() {
                buttons_pressed_for_switching_headless_on_off++;
                switch_headless_off_if_all_buttons_pressed(
                        buttons_pressed_for_switching_headless_on_off);
            });
    gamepad1.left_bumper.add_button_release_listener(
            []() { buttons_pressed_for_switching_headless_on_off--; });

    gamepad1.right_bumper.add_button_press_listener(
            []() {
                buttons_pressed_for_switching_headless_on_off++;
                switch_headless_off_if_all_buttons_pressed(
                        buttons_pressed_for_switching_headless_on_off);
            });
    gamepad1.right_bumper.add_button_release_listener(
            []() { buttons_pressed_for_switching_headless_on_off--; });

    gamepad1.right_trigger.add_full_press_listener(
            [](float value) { drive_fast = true; });
    gamepad1.right_trigger.add_trigger_release_listener(
            [](float value) { drive_fast = false; });

    gamepad1.right_bumper.add_button_press_listener([]() { intake::activate_intake(); });
    gamepad1.right_bumper.add_button_release_listener([]() { intake::deactivate_intake(); });

    gamepad1.left_bumper.add_button_press_listener([]() { intake::activate_intake_reversed(); });
    gamepad1.left_bumper.add_button_release_listener([]() { intake::deactivate_intake(); });

    gamepad1.dpad_right.add_button_press_listener([]() {
        if (suspension_arm_and_drone_launcher::get_elevation_in_degrees() > 10) {
            suspension_arm_and_drone_launcher::launch_drone();
        }
    });
    gamepad1.dpad_left.add_button_press_listener(
            []() { suspension_arm_and_drone_launcher::set_target_elevation_in_degrees(90); });
    gamepad1.dpad_up.add_button_press_listener([]() {
        elevate_arm = true;
        thread([] {
            while (elevate_arm) {
                suspension_arm_and_drone_launcher::set_target_elevation_in_degrees(
                        suspension_arm_and_drone_launcher::get_elevation_in_degrees() + 2);
            }
        }).detach();
    });
    gamepad1.dpad_up.add_button_release_listener([]() {
        elevate_arm = false;
    });
    gamepad1.dpad_down.add_button_press_listener([]() {
        elevate_arm = true;
        thread([] {
            while (elevate_arm) {
                suspension_arm_and_drone_launcher::set_target_elevation_in_degrees(
                        suspension_arm_and_drone_launcher::get_elevation_in_degrees() - 2);
            }
        }).detach();
    });
    gamepad1.dpad_down.add_button_release_listener([]() {
        elevate_arm = false;
    });

    drivetrain::preset_for_tele_op();

    telemetry::add_line("initialized");
    telemetry::update();
    wait_for_start();
    gamepad1.rumble(300);

    auto start_time = chrono::high_resolution_clock::now();
    while (op_mode_is_active()) {
        Position current_position = track_position::get_position();
        Vec2d current_speed = track_position::get_speed();

        telemetry::add_data("position", "x: " + to_string(current_position.x) + " y: " +
                                        to_string(current_position.y) + " rotation: " +
                                        to_string(current_position.rotation));
        telemetry::add_data("speed", "x: " + to_string(current_speed.x) + " y: " +
                                     to_string(current_speed.y));
        telemetry::add_data("loop time", (int) chrono::duration_cast<chrono::microseconds>(
                chrono::high_resolution_clock::now() - start_time).count());
        telemetry::update();
        start_time = chrono::high_resolution_clock::now();
        gamepads::update();

        if (!drive_to_position_mode)
            drivetrain::do_drive();
    }

    stop();
}