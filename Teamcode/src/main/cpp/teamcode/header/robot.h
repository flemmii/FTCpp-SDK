//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_ROBOT_H
#define FTCROBOTCONTROLLER_ROBOT_H

#include "sdk/hardware/Lynx_module.h"
#include "sdk/hardware/hardware_map.h"
#include "sdk/hardware/BNO055IMU.h"
#include "sdk/hardware/camera/Webcam_name.h"

#include "drivetrain.h"
#include "elevator.h"
#include "intake.h"
#include "last_position_storage.h"
#include "output.h"
#include "suspension_arm_and_drone_launcher.h"
#include "track_position.h"

// In kilograms
#define WEIGHT_OF_ROBOT 10 // in kg
#define GEAR_RATIO_DRIVETRAIN 20
#define TRACK_WIDTH 364.4 // in mm

namespace robot {
    extern sdk::Lynx_module control_hub;
    extern sdk::Lynx_module expansion_hub;

    extern class sdk::BNO055IMU imu;

    extern sdk::Webcam_name front_webcam;
    extern sdk::Webcam_name rear_webcam;

    void init();
} // robot

#endif //FTCROBOTCONTROLLER_ROBOT_H
