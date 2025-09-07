#pragma once

#include <optional>

#include "Units-master/Units.h"
#include "constants.hpp"

namespace intake::wrist
{
    constexpr double SERVO_WRIST_RATIO = 18.0 / 16;
    namespace servo_type
    {
        constexpr Degree RANGE_OF_MOTION = servos::goBilda::dual_mode::RANGE_OF_MOTION * SERVO_WRIST_RATIO;
        constexpr sdk::Pwm_control::Pwm_range PWM_RANGE = servos::goBilda::dual_mode::PWM_RANGE;
    }
    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = (60_deg / 90_ms) * SERVO_WRIST_RATIO;

    // This gets added on every given angle
    // So if it has 10° but should be 0°, you should set it to -10°
    constexpr Degree OFFSET = 7_deg;
    constexpr Degree MIN_ROTATION = -95_deg;
    constexpr Degree MAX_ROTATION = 110_deg;

    constexpr Degree TRANSFER_ROTATION = -180_deg;

    extern Degree manual_set_target_rotation;
    // When arm is not moving this is -arm::target_rotation
    extern Degree coaxial_target_rotation;
    extern bool moving;
    extern int method_count;

    void init();

    void stop();

    bool update_rotation(Degree coaxial_rotation, bool wait_until_done = false);

    bool set_rotation(Degree rotation, bool wait_until_done = false, std::optional<Degree> coaxial_rotation = std::nullopt);

    bool rotate_to_transfer(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();
} // wrist
