#pragma once

#include "Units-master/Units.h"
#include "utils/Vec2d.hpp"
#include "constants.hpp"

namespace clipper::intake
{
    constexpr double SERVO_INTAKE_RATIO = 2;

    namespace servo_type
    {
        constexpr Degree RANGE_OF_MOTION = servos::rev_smart::RANGE_OF_MOTION * SERVO_INTAKE_RATIO;
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE = servos::rev_smart::PWM_RANGE;
    }

    // This gets added on every given angle
    // So if it has 10° but should be 0°, you should set it to -10°
    constexpr Degree OFFSET = -25_deg;

    constexpr Degree MIN_ROTATION = -180_deg;
    constexpr Degree MAX_ROTATION = 0_deg;

    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = (60_deg / 400_ms) * SERVO_INTAKE_RATIO;

    constexpr Degree LOWER_INTAKE_POSITION = -0_deg;
    constexpr Degree UPPER_INTAKE_POSITION = -50_deg;
    constexpr Degree STORAGE_POSITION = -90_deg;
    constexpr Degree DEPOSIT_POSITION = -180_deg;

    extern Degree target_rotation;
    extern Degree last_target_rotation;
    extern bool moving;
    extern int method_count;

    void init();

    void stop();

    Degree est_current_pos();

    std::chrono::steady_clock::duration time_needed_to_reach_goal(const Degree &target);

    bool set_rotation(Degree rotation, bool wait_until_done = false);

    bool move_to_lower_intake_position(bool wait_until_done = false);

    bool move_to_upper_intake_position(bool wait_until_done = false);

    bool move_to_storage_position(bool wait_until_done = false);

    bool move_to_deposit_position(bool wait_until_done = false);

    void wait_for_movement_finish();
} // intake