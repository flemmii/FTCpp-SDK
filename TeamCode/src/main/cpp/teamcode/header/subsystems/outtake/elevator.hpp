#pragma once

#include "Units-master/Units.h"
#include "Units-master/Maths.h"

#include "constants.hpp"

namespace outtake::elevator
{
    constexpr Degree ANGLE = 10_deg;
    constexpr Millimetre MAX_LENGTH = 682_mm;

    constexpr Kilogram MASS(2.5);
    constexpr double GEAR_RATIO = 3.61 * 3.61;
    constexpr Millimetre SPOOL_DIAMETER = 32_mm; // in mm

    constexpr Divide<Millimetre, Tick>::Type MM_PER_TICK = (SPOOL_DIAMETER * M_PI / 1_rev) / (GEAR_RATIO * motors::TICKS_PER_REVOLUTION);

    extern Power max_motor_power;
    extern Millimetre target_elevation;
    extern bool moving;
    extern bool encoder_working;
    extern int method_count;

    void init();

    void stop();

    template <typename Time_unit = Real>
    constexpr typename Divide<Millimetre, Time_unit>::Type conv_elevation_to_length(const typename Divide<Millimetre, Time_unit>::Type &elevation)
    {
        return elevation / maths::Cos(ANGLE);
    }

    template <typename Time_unit = Real>
    constexpr typename Divide<Millimetre, Time_unit>::Type conv_to_elevation(const typename Divide<Millimetre, Time_unit>::Type &length)
    {
        return length * maths::Cos(ANGLE);
    }

    template <typename Time_unit = Real>
    constexpr typename Divide<Millimetre, Time_unit>::Type conv_to_extension(const typename Divide<Millimetre, Time_unit>::Type &length)
    {
        return length * maths::Sin(ANGLE);
    }

    const Millimetre MAX_ELEVATION = conv_to_elevation(MAX_LENGTH);

    Millimetre get_length();

    Speed get_speed();

    Millimetre get_elevation();

    Speed get_elevation_speed();

    // USE THIS WITH CAUTION, NO PROTECTION AGAINST ANYTHING
    void set_power_override(Power power);

    Power get_power();

    bool homed();

    bool reset_position(bool wait_until_done = false);

    bool home(bool wait_until_done = false);

    void set_power(Power power);

    bool is_hanging();

    Kilogram get_mass();

    bool set_elevation(Millimetre mm, bool wait_until_done = false, bool hang = false);

    bool retract(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();
} // elevator