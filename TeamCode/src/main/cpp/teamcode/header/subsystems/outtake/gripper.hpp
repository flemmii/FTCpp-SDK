#pragma once

#include "Units-master/Units.h"
#include "constants.hpp"

namespace outtake::gripper
{
    namespace servo_type = servos::rev_smart;

    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = (60_deg / 250_ms);

    enum class Status
    {
        OPEN,
        CLOSED,
        CLOSED_LOSELY
    };

    extern Status status;
    extern bool moving;
    extern int method_count;

    void init();

    void stop();

    bool open(bool wait_until_done = false);

    bool close(bool wait_until_done = false, bool open_intake_if_needed = true);

    bool close_losely(bool wait_until_done = false);

    void wait_for_movement_finish();
} // gripper
