//
// Created by fnlg on 28.12.2023.
//

#include "../../../header/opmodes/tele_op/GeometricalDriveTest.h"

using namespace std;
using namespace sdk;
using namespace linear_op_mode;
using namespace gamepads;


extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_GeometricalDriveTest_opMode(
        JNIEnv *env, jobject thiz) {

    init_sdk

    /*
    Dc_motor_ex intake(hardware_map::get(DcMotorEx, "intake"));
    Position *DrivingPos;
    unsigned int OR[2] = {100, 120};
    unsigned int TL[2] = {100, 200};
    int RP[2] = {0, 0};
    int RM[2] = {20, 20};
    geometrical_drive::setObstacle(OR, OR);
    geometrical_drive::setTargetLocation(TL);
    geometrical_drive::setRobotLocation(RP, RM);
    DrivingPos = geometrical_drive::getCompatibleDrivingPoints();
    telemetry::set_auto_clear(false);


    wait_for_start();
    telemetry::add_line("Telemetry-test");
    for (int i = 0; i < sizeof(DrivingPos); i++) {
        telemetry::add_line(
                "Position: " + to_string(i) + " x: " + to_string(DrivingPos[i].x) + ", y: " +
                to_string(DrivingPos[i].y) + ";");
        telemetry::update();
    }
    telemetry::add_line("");
    auto start_time = chrono::high_resolution_clock::now();
    while (op_mode_is_active()) {
        start_time = chrono::high_resolution_clock::now();


        gamepads::update();

    }
     */

    stop();

}