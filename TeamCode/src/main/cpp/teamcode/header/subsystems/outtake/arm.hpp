#pragma once

#include "Units-master/Units.h"
#include "Units-master/Maths.h"
#include "utils/Vec3d.hpp"
#include "constants.hpp"
#include "subsystems/outtake/elevator.hpp"

namespace outtake::arm
{
    // This gets added on every given angle
    // So if it has 10° but should be 0°, you should set it to -10°
    constexpr Degree MAX_ROTATION = 150_deg;
    constexpr Degree MIN_ROTATION(-95.5);
    constexpr Degree OFFSET = -168_deg + elevator::ANGLE;
    constexpr Millimetre MAX_REACH_EXTENSION_LIMIT = 130_mm;

    constexpr Millimetre BASE_LENGTH(221.66025);              // center of rotation arm to center of rotation wrist
    constexpr Torque TORQUE_ON_ARM_AT_ZERO_EXTENSION(0.8835); // At 0°
    constexpr Torque TORQUE_ON_ARM_AT_MAX_EXTENSION(1.493);   // At 0°

    constexpr double ARM_MOTOR_RATIO = 4;
    constexpr double GEAR_RATIO = (((1 + (46.0 / 17))) * (1 + (46.0 / 17))) * ARM_MOTOR_RATIO;

    constexpr typename Divide<Degree, Volt>::Type DEG_PER_VOLT(360 / 3.3);

    constexpr Divide<Degree, Tick>::Type DEG_PER_TICK = 360_deg / (GEAR_RATIO * motors::TICKS_PER_REVOLUTION * 1_rev);

    constexpr Degree TRANSFER_ROTATION = -10_deg;

    extern Power max_motor_power;
    extern Degree target_rotation;
    extern bool moving;
    extern bool encoder_working;
    extern int method_count;

    void init();

    void stop();

    Millimetre get_length();

    Millimetre calc_reach(Degree rotation);

    Millimetre calc_height(Degree rotation);

    Degree get_rotation();

    AngularSpeed get_rotation_speed();

    void set_power(Power power);

    bool set_rotation(Degree rotation, bool wait_until_done = false);

    bool move_to_transfer_position(bool wait_until_done = false);

    void wait_for_movement_finish();

    bool ready_for_transfer();

    namespace linkage
    {
        constexpr Millimetre SERVO_ATTACHED_PART_LENGTH(104); // center to center distance
        constexpr Millimetre SLIDE_ATTACHED_PART_LENGTH(96);  // center to center distance
        constexpr Vec3d<Millimetre> ZERO_OFFSET(0_mm, -17476820_nm, 36_mm);

        const Millimetre MAX_EXTENSION = maths::Sqrt(maths::Pow<2>(SERVO_ATTACHED_PART_LENGTH + SLIDE_ATTACHED_PART_LENGTH) -
                                                     maths::Pow<2>(ZERO_OFFSET.z)) -
                                         ZERO_OFFSET.y;

        constexpr Divide<Degree, Millisecond>::Type MAX_VELOCITY = 60_deg / 250_ms;

        // This gets added on every given angle
        // So if it has 10° but should be 0°, you should set it to -10°
        constexpr Degree OFFSET = 7_deg;

        constexpr Degree ROTATION_OVERCENTERED = 185_deg;

        extern Millimetre target_extension;
        extern bool moving;
        extern int method_count;

        void init();

        void stop();

        bool set_length(Millimetre length, bool wait_until_done = false);

        bool retract(bool wait_until_done = false);

        bool overcenter(bool wait_until_done = false);

        void wait_for_movement_finish();
    }

    const Millimetre MAX_LENGTH = BASE_LENGTH + linkage::MAX_EXTENSION; // Around 431mm
} // arm