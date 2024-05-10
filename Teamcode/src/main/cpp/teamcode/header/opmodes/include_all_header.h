//
// Created by fnlg on 09.05.2024.
//

#ifndef FTCROBOTCONTROLLER_INCLUDE_ALL_HEADER_H
#define FTCROBOTCONTROLLER_INCLUDE_ALL_HEADER_H

#include <chrono>
#include <jni.h>
#include <thread>
#include <vector>

// ------------------- SDK -------------------

#include "../../sdk/header/init_sdk.h"
#include "../../sdk/header/telemetry.h"

#include "sdk/hardware/Analog_input.h"
#include "sdk/hardware/BNO055IMU.h"
#include "sdk/hardware/Color_sensor.h"
#include "sdk/hardware/CR_servo.h"
#include "sdk/hardware/Dc_motor_ex.h"
#include "sdk/hardware/Dc_motor_simple.h"
#include "sdk/hardware/Digital_channel.h"
#include "sdk/hardware/Distance_sensor.h"
#include "sdk/hardware/Gyro_sensor.h"
#include "sdk/hardware/hardware_map.h"
#include "sdk/hardware/Lynx_module.h"
#include "sdk/hardware/PIDF_coefficients.h"
#include "sdk/hardware/Servo.h"
#include "sdk/hardware/Servo_controller.h"
#include "sdk/hardware/Touch_sensor.h"

#include "sdk/hardware/camera/Webcam_name.h"
#include "sdk/hardware/camera/Camera_name.h"

#include "sdk/hardware/Gamepad/Analog_stick.h"
#include "sdk/hardware/Gamepad/Analog_sticks.h"
#include "sdk/hardware/Gamepad/Button.h"
#include "sdk/hardware/Gamepad/Gamepad.h"
#include "sdk/hardware/Gamepad/Trigger.h"

#include "sdk/navigation/Voltage_unit.h"

#include "sdk/opmode/linear_op_mode.h"

#include "sdk/vision/Vision_portal.h"
#include "sdk/vision/Vision_processor.h"

// ------------------- Teamcode -------------------

#include "../drive_to_position_controller.h"
#include "../drivetrain.h"
#include "../elevator.h"
#include "../error_detection.h"
#include "../intake.h"
#include "../last_position_storage.h"
#include "../match_data.h"
#include "../output.h"
#include "../PID_controller.h"
#include "../Position.h"
#include "../robot.h"
#include "../suspension_arm_and_drone_launcher.h"
#include "../track_position.h"
#include "extras/utils.h"
#include "../Vec2d.h"
#include "extras/Vec3d.h"

#include "../vision/picture_normalization.h"
#include "../vision/Team_prop_detection.h"

#endif //FTCROBOTCONTROLLER_INCLUDE_ALL_HEADER_H
