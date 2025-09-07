#pragma once

#include "Units-master/Units.h"
#include "utils/Vec2d.hpp"

namespace intake
{
    constexpr Millimetre EXTENSION_IN_SUB_TRANSFER = 100_mm;
    constexpr Millimetre CAMERA_OFFSET_SIDEWARDS = 73_mm;
    constexpr Millimetre DISTANCE_EXTENDO_SWIVEL = -36500_um;

    constexpr tuple<Degree, Degree> INTERFERENCE_ROTATION_ARM(0_deg, -35_deg);
    constexpr Degree INTERFERENCE_ROTATION_SWIVEL = 47_deg;

    constexpr Millimetre TRANSFER_EXTENSION = 75_mm;

    extern bool interrupted_cycle;

    void init();

    void stop();

    Millimetre get_extension();

    std::tuple<Millimetre, Degree> conv_to_extension_and_rotation(Vec2d<false> pos);

    bool reset(bool wait_until_done = false);

    bool set_position(Vec2d<false> pos, Degree rotation_arm, Degree rotation_wrist, Degree rotation_gripper, bool wait_until_done = false);

    bool move_to_hovering_position(Vec2d<false> pos, bool wait_until_done = false);

    bool move_to_transfer_position(bool wait_until_done = false);

    bool move_to_fast_transfer_position(bool wait_until_done = false);

    // If check_for_correct_color is false it only checks if there is a sample
    bool grip_sample(bool wait_until_done = false, bool check_for_correct_color = true);

    bool search_sample(Vec2d<false> start_pos, Degree rotation_wrist, bool wait_until_done = false);

    bool move_to_in_sub_transfer_position(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();

    // cam

    Millimetre get_camera_height();
    Degree get_camera_angle();

} // intake
