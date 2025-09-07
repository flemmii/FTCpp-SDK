#pragma once

#include "Units-master/Units.h"
#include "sdk.hpp"

constexpr ::Acceleration GRAVITY(9.80665); // Standard-Erdbeschleunigung

namespace motors
{
    // Currently: AndyMark Neverest and REV HD Hex Motor
    constexpr Divide<Tick, Revolution>::Type TICKS_PER_REVOLUTION(28); // These are the ticks per revolution of a bare motor. For motors with gearboxes, this number should be multiplied by the gearbox ratio
    constexpr Divide<Torque, Ampere>::Type TORQUE_CONSTANT(0.108);
    constexpr Divide<Volt, Divide<Radian, Second>::Type>::Type COUNTER_ELECTROMOTIVE_FORCE(0.0174);
    constexpr Ohm RESISTANCE(5);
} // motors

namespace servos
{
    namespace rev_smart
    {
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(500, 2500);
        constexpr Degree RANGE_OF_MOTION = 270_deg;
    }

    namespace goBilda
    {
        namespace dual_mode
        {
            constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(500, 2500);
            constexpr Degree RANGE_OF_MOTION = 300_deg;
        }

        namespace five_turn
        {
            constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(500, 2500);
            constexpr Degree RANGE_OF_MOTION = 5_rev;
        }

        namespace proton
        {
            constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(600, 2400);
            constexpr Degree RANGE_OF_MOTION = 180_deg;
        }
    }

    namespace plex
    {
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(500, 2500);
        constexpr Degree RANGE_OF_MOTION = 270_deg;
    }

    namespace ds
    {
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(500, 2500);
        constexpr Degree RANGE_OF_MOTION = 270_deg;
    }

    namespace hitec
    {
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(553, 2425);
        constexpr Degree RANGE_OF_MOTION = 180_deg;
    }

    namespace miuzei
    {
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE(500, 2500);
        constexpr Degree RANGE_OF_MOTION = 180_deg;
    }
}

namespace encoder
{
    // Currently: REV Through Bore Encoder
    constexpr auto TICKS_PER_REVOLUTION = 8192_tick / 1_rev; // REV Through Bore Encoder
}

namespace camera
{
    // Currently: Logitech C270
    constexpr Millimetre FOCAL_LENGTH = 4_mm;
    constexpr Millimetre SENSOR_HEIGHT = 2700_um;
} // camera

namespace slides
{
    constexpr Gram MASS(101.2);
}