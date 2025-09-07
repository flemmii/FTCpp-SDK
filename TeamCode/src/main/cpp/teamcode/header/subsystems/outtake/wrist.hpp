#pragma once

#include <optional>

#include "Units-master/Units.h"
#include "constants.hpp"

namespace outtake::wrist
{
    namespace servo_type = servos::goBilda::dual_mode;

    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = 60_deg / 200_ms;

    // This gets added on every given angle
    // So if it has 10° but should be 0°, you should set it to -10°
    constexpr Degree OFFSET = 10_deg + (360_deg / 17) * 0;
    constexpr Degree MIN_ROTATION = -90_deg;
    constexpr Degree MAX_ROTATION = 85_deg;
    constexpr Degree TRANSFER_ROTATION = 0_deg;

    extern Degree manual_set_target_rotation;
    // When arm is not moving this is -arm::target_rotation
    extern Degree coaxial_target_rotation;
    extern bool moving;
    extern int method_count;

    void init();

    void stop();

    bool update_rotation(Degree coaxial_rotation, bool wait_until_done = false);

    bool set_rotation(Degree rotation, bool wait_until_done = false, std::optional<Degree> coaxial_rotation = std::nullopt, bool clipping = false);

    bool rotate_to_transfer(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();
} // wrist
