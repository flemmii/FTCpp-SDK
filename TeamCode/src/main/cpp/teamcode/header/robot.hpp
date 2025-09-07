#pragma once

#include "sdk.hpp"

#include "Units-master/Units.h"
#include "utils/Vec3d.hpp"
#include "vision/Sample_detection.hpp"

namespace robot
{
    constexpr Kilogram MASS(12);
    constexpr Vec3d<Millimetre> SIZE(370_mm, 400_mm, 390_mm);
    constexpr Vec3d<> DISTANCE_OUTTAKE_INTAKE(0_mm, 301788820_nm, -183226720_nm);

    extern sdk::Lynx_module control_hub;
    extern sdk::Lynx_module expansion_hub;

    extern sdk::BNO055IMU imu;

    extern sdk::Webcam_name webcam;

    extern sdk::Vision_portal vision_portal;
    extern Sample_detection *sample_detection;

    extern bool scoring_samples; // Otherwise we are scoring specimen

    void init(bool do_initial_reset = true);

    void stop();

    bool transfer(bool wait_until_done = false);

    bool ready_for_transfer();

    void wait_for_movement_finish();
} // robot
