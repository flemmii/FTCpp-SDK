//
// Created by Frosch PC - V2 on 19.03.2024.
//

#include "../../../header/opmodes/autonomous/AutoTest.h"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace drive_to_position_controller;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_autonomous_AutoTest_opMode(JNIEnv *env,
                                                                       jobject thiz) {
    init_sdk
    robot::init();
    drivetrain::preset_for_auto();
    set_max_motor_power(1);
    vector<int> ids = Vision_portal::makeMultiPortalView(2,
                                                         Vision_portal::Multi_portal_layout::VERTICAL);
    Vision_portal vision_portal = Vision_portal::Builder().set_live_view_container_id(
                    ids[0])
            .set_camera(robot::front_webcam)
            .add_processor(Team_prop_detection())
            .set_camera_resolution(Vision_portal::Size(640, 480))
            .set_auto_stop_live_view(true)
            .build();

    Vision_portal vision_portal2 = Vision_portal::Builder().set_live_view_container_id(
                    ids[1])
            .set_camera(robot::rear_webcam)
            .add_processor(Team_prop_detection())
            .set_camera_resolution(Vision_portal::Size(640, 480))
            .set_auto_stop_live_view(true)
            .build();

    Vision_portal::enable_dual_cam_view();

    telemetry::add_line("Initialized");
    telemetry::update();

    wait_for_start();

    if (op_mode_is_active()) {
        //drive_to_position({{0, 500}, {500, 500}}, true, 5000);
        //drive_to_position(0, 0, 0, 5000);
        drive_to_position({{0,    700},
                           {-500, 700}}, true, 3000);
        this_thread::sleep_for(chrono::milliseconds(2000));
        drive_to_position({{-900, 700}}, true, 3000);
        this_thread::sleep_for(chrono::milliseconds(2000));
        drive_to_position({{-300, 1500},
                           {1200, 1300},
                           {1800, 1300}}, false, 5000);
        this_thread::sleep_for(chrono::milliseconds(2000));
    }

    linear_op_mode::stop();
}