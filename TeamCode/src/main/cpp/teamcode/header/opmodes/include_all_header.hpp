//
// Created by fnlg on 09.05.2024.
//

#ifndef FTCROBOTCONTROLLER_INCLUDE_ALL_HEADER_H
#define FTCROBOTCONTROLLER_INCLUDE_ALL_HEADER_H

#include <chrono>
#include <jni.h>
#include <vector>
#include <semaphore.h>

// ------------------- Units -------------------

#include "Units-master/Units.h"
#include "Units-master/Maths.h"

// ------------------- SDK -------------------

#include "sdk.hpp"

// ------------------- Teamcode -------------------

#include "constants.hpp"
#include "field.hpp"
#include "match_data.hpp"
#include "task_scheduler.hpp"
#include "robot.hpp"

#include "Gamepad/Sticks.hpp"
#include "Gamepad/Button.hpp"
#include "Gamepad/Better_gamepad.hpp"
#include "Gamepad/Trigger.hpp"

#include "opmodes/autonomous/sample_preset.hpp"
#include "opmodes/autonomous/specimen_preset.hpp"

#include "subsystems/error_detection.hpp"
#include "subsystems/subsystem_utils.hpp"

#include "subsystems/clipper/aligner.hpp"
#include "subsystems/clipper/clipper.hpp"
#include "subsystems/clipper/intake.hpp"

#include "subsystems/driving/drive_to_position_controller.hpp"
#include "subsystems/driving/drivetrain.hpp"
#include "subsystems/driving/last_position_storage.hpp"
#include "subsystems/driving/pinpoint.hpp"
#include "subsystems/driving/pinpoint.hpp"

#include "subsystems/intake/arm.hpp"
#include "subsystems/intake/extendo.hpp"
#include "subsystems/intake/gripper.hpp"
#include "subsystems/intake/intake.hpp"
#include "subsystems/intake/swivel.hpp"
#include "subsystems/intake/wrist.hpp"

#include "subsystems/outtake/arm.hpp"
#include "subsystems/outtake/elevator.hpp"
#include "subsystems/outtake/gripper.hpp"
#include "subsystems/outtake/outtake.hpp"
#include "subsystems/outtake/wrist.hpp"

#include "utils/Location.hpp"
#include "utils/PID_controller.hpp"
#include "utils/Position.hpp"
#include "utils/Vec2d.hpp"
#include "utils/Vec3d.hpp"

#include "vision/picture_utils.hpp"
#include "vision/Sample_detection.hpp"
#include "vision/Team_prop_detection.hpp"

#endif // FTCROBOTCONTROLLER_INCLUDE_ALL_HEADER_H
