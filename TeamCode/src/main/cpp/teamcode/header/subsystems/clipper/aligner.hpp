#pragma once

#include "Units-master/Units.h"
#include "utils/Vec2d.hpp"
#include "constants.hpp"

namespace clipper::aligner
{
    constexpr double SERVO_aligner_RATIO = 3;

    namespace servo_type
    {
        constexpr Degree RANGE_OF_MOTION = servos::rev_smart::RANGE_OF_MOTION * SERVO_aligner_RATIO;
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE = servos::rev_smart::PWM_RANGE;
    }

    constexpr Millimetre PULLEY_DIAMETER = 18340_um;
    constexpr auto MM_PER_DEGREE = (PULLEY_DIAMETER * M_PI) / 1_rev;

    constexpr Millimetre MIN_POSITION = 0_mm;
    constexpr Millimetre MAX_POSITION = 100_mm;

    constexpr auto MAX_VELOCITY = (60_deg * MM_PER_DEGREE) / 180_ms * SERVO_aligner_RATIO;

    extern int current_clip_count;

    namespace right
    {
        extern sdk::Servo_impl_ex servo;

        // This gets added on every given angle
        // So if it has 10° but should be 0°, you should set it to -10°
        constexpr Millimetre OFFSET = -4_mm;

        extern Millimetre target_position;
        extern Millimetre last_target_position;
        extern bool moving;
        extern int method_count;

        void init();

        void stop();

        bool set_position(Millimetre position, bool wait_until_done = false);

        void wait_for_movement_finish();
    }

    namespace left
    {
        // This gets added on every given angle
        // So if it has 10° but should be 0°, you should set it to -10°
        constexpr Millimetre OFFSET = -21_mm;

        extern Millimetre target_position;
        extern Millimetre last_target_position;
        extern bool moving;
        extern int method_count;

        void init();

        void stop();

        bool set_position(Millimetre position, bool wait_until_done = false);

        void wait_for_movement_finish();
    }

    void init();

    void stop();

    bool set_position(Millimetre position, bool wait_until_done = false);

    bool setup_for_clips(int clips, bool wait_until_done = false);

    void wait_for_movement_finish();
} // aligner