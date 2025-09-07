#pragma once

#include "Units-master/Units.h"
#include "Units-master/Maths.h"
#include "constants.hpp"

namespace intake::arm
{
    namespace servo_type = servos::plex;

    constexpr Degree MIN_ROTATION = -100_deg;
    constexpr Degree MAX_ROTATION = 20_deg;

    constexpr Millimetre LENGTH = 170_mm;
    constexpr Millimetre DISTANCE_SWIVEL_ARM = 18800_um;

    constexpr Degree TRANSFER_ROTATION = -90_deg;
    constexpr Degree SEARCHING_ROTATION = -60_deg;
    constexpr Degree HOVERING_ROTATION = -25_deg;
    constexpr Degree GRIPPING_ROTATION = 8_deg;

    constexpr Millimetre AXLE_GROUND_HEIGHT = 82_mm;

    constexpr Millimetre GRIPPING_RADIUS = DISTANCE_SWIVEL_ARM + LENGTH; // * maths::Cos(GRIPPING_ROTATION) = 1;

    // This gets added on every given angle
    // So if it has 10° but should be 0°, you should set it to -10°
    constexpr Degree OFFSET = 5_deg;

    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = 60_deg / 100_ms;

    extern Degree target_rotation;
    extern Degree last_target_rotation;
    extern bool moving;
    extern int method_count;

    void init();

    void stop();

    Degree est_current_pos();

    std::chrono::steady_clock::duration time_needed_to_reach_goal(const Degree &target);

    bool set_rotation(Degree rotation, bool wait_until_done = false);

    bool move_to_transfer_position(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();

} // arm