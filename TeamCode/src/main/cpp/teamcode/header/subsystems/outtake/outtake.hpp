#pragma once

#include <tuple>

#include "Units-master/Units.h"

namespace outtake
{
    constexpr Millimetre INTERFERENCE_EXTENSION_ARM_LINKAGE = 15_mm;
    constexpr Degree INTERFERENCE_ROTATION_WRIST = -75_deg;

    constexpr Millimetre MIN_HEIGHT_ALLOWED = -220_mm; // height of transfer position

    constexpr Millimetre HIGH_BASKET_HEIGHT = 910_mm;
    constexpr Millimetre LOW_BASKET_HEIGHT = 500_mm;

    constexpr Millimetre HIGH_CHAMBER_ELEVATION = 450_mm;
    constexpr Millimetre SPECIMEN_INTAKE_HEIGHT = 0_mm;
    constexpr Millimetre SPECIMEN_INTAKE_REACH = 50_mm;

    constexpr Millimetre CLIPPING_HEIGHT = -80_mm;
    constexpr Millimetre CLIPPING_REACH = -120_mm;

    extern Degree BASKET_ROTATION;

    extern bool interrupted_cycle;

    void init();

    void stop();

    Millimetre get_height();

    std::tuple<Millimetre, Degree> conv_to_elevation_and_rotation(const Millimetre &height, const Millimetre &reach, const Millimetre &linkage_length);

    std::tuple<Millimetre, Degree> conv_to_elevation_and_rotation(const Millimetre &height, const Millimetre &reach);

    std::tuple<Degree, Millimetre> conv_to_rotation_and_length(const Millimetre &height_arm, const Millimetre &reach_arm);

    bool reset(bool wait_until_done = false);

    bool set_position(Millimetre height, Millimetre reach, Degree rotation_wrist, bool wait_until_done = false);

    bool move_to_rest_position(bool wait_until_done = false);

    bool move_to_transfer_position(bool wait_until_done = false);

    bool move_to_fast_transfer_position(bool wait_until_done = false);

    bool move_to_high_basket_position(bool wait_until_done = false);

    bool move_to_low_basket_position(bool wait_until_done = false);

    bool get_specimen_and_move_to_high_chamber_position(bool wait_until_done = false);

    bool get_specimen_and_move_to_high_chamber_position_auto(bool wait_until_done = false);

    bool move_to_specimen_intake_position(bool wait_until_done = false);

    bool move_to_specimen_intake_position_auto(bool wait_until_done = false);

    bool move_to_in_sub_transfer_position(bool wait_until_done = false);

    bool clip(bool wait_until_done = false);

    bool clip_and_score(bool wait_until_done = false);

    bool is_spec_in_aligner();

    bool grab_spec_when_in_aligner(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();
} // outtake