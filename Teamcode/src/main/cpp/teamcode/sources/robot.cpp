//
// Created by fnlg on 27.01.2024.
//

#include "../header/robot.h"

using namespace sdk;

namespace robot {
    Lynx_module control_hub;
    Lynx_module expansion_hub;

    Webcam_name front_webcam;
    Webcam_name rear_webcam;

    class BNO055IMU imu;

    void init() {
        control_hub = hardware_map::get(LynxModule, "Control Hub");
        expansion_hub = hardware_map::get(LynxModule, "Expansion Hub 2");
        imu = {hardware_map::get(BNO055IMU, "imu1"), Vec3d::Vec3d_remap_order::ZYX_XYZ};
        control_hub.set_bulk_caching_mode(Lynx_module::MANUAL);
        expansion_hub.set_bulk_caching_mode(Lynx_module::MANUAL);
        front_webcam = hardware_map::get(WebcamName, "front_webcam");
        rear_webcam = hardware_map::get(WebcamName, "rear_webcam");

        drivetrain::init();
        elevator::init();
        intake::init();
        output::init();
        suspension_arm_and_drone_launcher::init();
        track_position::init();
    }
} // robot