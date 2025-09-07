#pragma once

#include <optional>

#include "Units-master/Units.h"
#include "constants.hpp"

namespace intake::gripper
{
    namespace servo_type = servos::goBilda::dual_mode;

    constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = (60_deg / 150_ms);

    enum class Status
    {
        OPEN,
        CLOSED,
        CLOSED_LOSELY
    };

    extern Status status;
    extern bool closed;
    extern Degree target_rotation;
    extern bool moving;
    extern int method_count;

    enum class Sample_color
    {
        NONE,
        RED,
        BLUE,
        YELLOW
    };

    void init();

    void stop();

    bool open(bool wait_until_done = false);

    bool close_losely(bool wait_until_done = false);

    bool close(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool correct_sample();

    Sample_color get_sample_color();

    namespace swivel
    {
        namespace servo_type = servos::miuzei;

        // This gets added on every given angle
        // So if it has 10° but should be 0°, you should set it to -10°
        constexpr Degree OFFSET = 5_deg;

        constexpr Degree MIN_ROTATION = -90_deg;
        constexpr Degree MAX_ROTATION = 90_deg;

        constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = (60_deg / 180_ms);

        extern Degree manual_set_target_rotation;
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
    } // swivel
} // gripper