#pragma once

#include <chrono>

#include "hardware/GoBilda_pinpoint_driver.hpp"
#include "Units-master/Maths.h"
#include "utils/Position.hpp"
#include "drivetrain.hpp"

namespace pinpoint
{
    // Offset of the center of the robot to the dead wheel
    constexpr Millimetre DEAD_WHEEL_SIDEWARDS_OFFSET_Y = 28_mm;     // (Forwards positive, backwards negative)
    constexpr Millimetre DEAD_WHEEL_FORWARDS_OFFSET_X = -144500_um; // (Right positive, left negative)

    constexpr sdk::GoBilda_pinpoint_driver::GoBilda_odometry_pods PODS = sdk::GoBilda_pinpoint_driver::GoBilda_odometry_pods::goBILDA_4_BAR_POD;
    constexpr sdk::GoBilda_pinpoint_driver::Encoder_direction DEAD_WHEEL_FORWARDS = sdk::GoBilda_pinpoint_driver::Encoder_direction::REVERSED;
    constexpr sdk::GoBilda_pinpoint_driver::Encoder_direction DEAD_WHEEL_SIDEWARDS = sdk::GoBilda_pinpoint_driver::Encoder_direction::FORWARD;

    constexpr std::chrono::steady_clock::duration MIN_QUERY_RATE = std::chrono::milliseconds(20);

    extern sdk::GoBilda_pinpoint_driver pinpoint;

    void init();

    void update_if_allowed();

    void set_position(const Position<> &position);

    void set_rotation(Degree rotation);

    Position<> get_position();

    Position<true, Speed, AngularSpeed> get_speed();

    Position<true, ::Acceleration, AngularAcceleration> get_acceleration();

    template <typename Time_unit = Real>
    constexpr typename Divide<Millimetre, Time_unit>::Type conv_to_distance(const typename Divide<Degree, Time_unit>::Type &rotation)
    {
        return (rotation / 360_deg) * M_PI * drivetrain::TRACK_WIDTH;
    }

    template <typename Time_unit = Real>
    constexpr typename Divide<Degree, Time_unit>::Type conv_to_rotation(const typename Divide<Millimetre, Time_unit>::Type &circumference)
    {
        return (circumference * 360_deg) / (drivetrain::TRACK_WIDTH * M_PI);
    }

    template <typename Unit>
    inline Vec2d<false, Unit> conv_to_robot_centric(const Vec2d<true, Unit> &powers)
    {
        const Radian rotation = pinpoint::get_position().rotation;
        const double sinus = maths::Sin(rotation);
        const double cosinus = maths::Cos(rotation);

        return {powers.y * cosinus + powers.x * sinus, -powers.y * sinus + powers.x * cosinus};
    }

    template <typename Unit>
    inline Vec2d<true, Unit> conv_to_field_centric(const Vec2d<false, Unit> &powers)
    {
        const Radian rotation = pinpoint::get_position().rotation;
        const double sinus = maths::Sin(rotation);
        const double cosinus = maths::Cos(rotation);

        return {powers.sidewards * cosinus + powers.forwards * sinus,
                powers.forwards * cosinus - powers.sidewards * sinus};
    }
}
