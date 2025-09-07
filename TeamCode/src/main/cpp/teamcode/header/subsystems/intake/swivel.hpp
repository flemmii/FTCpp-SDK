#pragma once

#include "Units-master/Units.h"
#include "utils/Vec2d.hpp"
#include "constants.hpp"

namespace intake::swivel
{
    namespace servo_type = servos::rev_smart;

    // This gets added on every given angle
    // So if it has 10° but should be 0°, you should set it to -10°
    constexpr Degree OFFSET = -6_deg;

    constexpr Degree MIN_ROTATION = -servo_type::RANGE_OF_MOTION / 2 - OFFSET;
    constexpr Degree MAX_ROTATION = servo_type::RANGE_OF_MOTION / 2 - OFFSET;

    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = 60_deg / 180_ms;

    extern Degree target_rotation;
    extern Degree last_target_rotation;
    extern bool moving;
    extern int method_count;

    void init();

    void stop();

    Degree est_current_pos();

    std::chrono::steady_clock::duration time_needed_to_reach_goal(const Degree &target);

    // Forwards extension when the arm is rotated to the gripping rotation
    Millimetre calc_extension_forwards(Degree rotation);

    // Sidewards extension when the arm is rotated to the gripping rotation
    Millimetre calc_extension_sidewards(Degree rotation);

    // Position of the arm relative to the extendo position when the arm is rotated to the gripping rotation
    Vec2d<false> calc_pos(Degree rotation);

    bool set_rotation(Degree rotation, bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();
} // swivel