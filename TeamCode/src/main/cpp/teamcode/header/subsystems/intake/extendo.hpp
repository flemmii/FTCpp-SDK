#pragma once

#include "Units-master/Units.h"
#include "constants.hpp"

namespace intake::extendo
{
    // This constant is lower than the actual maximum extension, due to extension limit
    constexpr Millimetre MAX_EXTENSION = 540_mm;
    constexpr double GEAR_RATIO = 3.25;
    constexpr Millimetre SPOOL_DIAMETER = 32_mm;

    constexpr Millimetre SPOOL_DIAMETER_HANG = 8900_um;
    constexpr double GEAR_RATIO_MULTIPLIER_HANG = 2;

    constexpr auto MM_PER_TICK = ((SPOOL_DIAMETER * M_PI) / 1_rev) / (GEAR_RATIO * motors::TICKS_PER_REVOLUTION);

    extern Power max_motor_power;
    extern Millimetre target_extension;
    extern bool moving;
    extern bool encoder_working;
    extern int method_count;

    void init();

    void stop();

    Millimetre get_extension();

    Speed get_extension_speed();

    bool homed();

    void set_power(Power power);

    bool reset_position(bool wait_until_done = false);

    bool home(bool wait_until_done = false);

    bool set_extension(Millimetre mm, bool wait_until_done = false, bool hang = false);

    bool retract(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();
} // extendo
