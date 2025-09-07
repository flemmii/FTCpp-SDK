#pragma once

#include <string>
#include <cmath>

#include "utils/Vec2d.hpp"
#include "Units-master/Units.h"

using namespace std;

template <bool field_centric = true, typename Unit_coordinates = Millimetre, typename Unit_rotation = Degree>
struct Position;

template <typename Unit_coordinates, typename Unit_rotation>
struct Position<true, Unit_coordinates, Unit_rotation>
{
    Unit_coordinates x{}, y{};
    Unit_rotation rotation{};

    constexpr Position() = default;

    template <typename Unit_coordinates2, typename Unit_rotation2>
    constexpr Position(const Position<true, Unit_coordinates2, Unit_rotation2> &position) : x(position.x), y(position.y), rotation(position.rotation) {}

    constexpr Position(const Unit_coordinates &x, const Unit_coordinates &y, const Unit_rotation &rotation) : x(x), y(y),
                                                                                                              rotation(rotation) {}

    constexpr Position(const Vec2d<true, Unit_coordinates> &vec, const Unit_rotation &rotation) : x(vec.x), y(vec.y),
                                                                                                  rotation(rotation) {}

    [[nodiscard]] constexpr Unit_coordinates total_distance() const
    {
        return sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr bool is_zero() const
    {
        return x == static_cast<Unit_coordinates>(0) && y == static_cast<Unit_coordinates>(0) && rotation == static_cast<Unit_rotation>(0);
    }

    [[nodiscard]] constexpr Vec2d<true, Unit_coordinates> to_vec() const
    {
        return {x, y};
    }

    friend constexpr Position operator-(const Position &lhs, const Position &rhs)
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.rotation - rhs.rotation};
    }

    friend constexpr Position operator+(const Position &lhs, const Position &rhs)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.rotation + rhs.rotation};
    }

    friend constexpr Position operator+(const Position &lhs, const Vec2d<true, Unit_coordinates> &rhs)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.rotation};
    }

    constexpr Position operator+=(const Position &rhs)
    {
        return *this = *this + rhs;
    }

    constexpr Position operator+=(const Vec2d<true, Unit_coordinates> &rhs)
    {
        return *this = *this + rhs;
    }

    friend constexpr Position operator-(const Position &lhs, const Vec2d<true, Unit_coordinates> &rhs)
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.rotation};
    }

    template <typename Unit_coordinates2, typename Unit_rotation2>
    friend constexpr Position<true, typename Multiply<Unit_coordinates, Unit_coordinates2>::Type, typename Multiply<Unit_rotation, Unit_rotation2>::Type>
    operator*(const Position &lhs, const Position<true, Unit_coordinates2, Unit_rotation2> &rhs)
    {
        return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.rotation * rhs.rotation};
    }

    template <typename Unit>
    friend constexpr Position<true, typename Multiply<Unit_coordinates, Unit>::Type, typename Multiply<Unit_rotation, Unit>::Type>
    operator*(const Position &lhs, const Unit &value)
    {
        return {lhs.x * value, lhs.y * value, lhs.rotation * value};
    }

    template <typename Unit_coordinates2, typename Unit_rotation2>
    friend constexpr Position<true, typename Divide<Unit_coordinates, Unit_coordinates2>::Type, typename Divide<Unit_rotation, Unit_rotation2>::Type>
    operator/(const Position &lhs, const Position &rhs)
    {
        return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.rotation / rhs.rotation};
    }

    template <typename Unit>
    friend constexpr Position<true, typename Divide<Unit_coordinates, Unit>::Type, typename Divide<Unit_rotation, Unit>::Type>
    operator/(const Position &lhs, const Unit &value)
    {
        return {lhs.x / value, lhs.y / value, lhs.rotation / value};
    }

    friend constexpr Position abs(const Position &lhs)
    {
        return {abs(lhs.x), abs(lhs.y), abs(lhs.rotation)};
    }

    friend constexpr Position sgn(const Position &lhs)
    {
        return {(lhs.x > 0) - (lhs.x < 0), (lhs.y < 0) - (lhs.y < 0), (lhs.rotation < 0) - (lhs.rotation < 0)};
    }

    // Comparison operators need to be in the header
    friend bool operator<(const Position &lhs, const Position &rhs)
    {
        return lhs.x < rhs.x &&
               lhs.y < rhs.y &&
               lhs.rotation < rhs.rotation;
    }

    friend bool operator==(const Position &lhs, const Position &rhs)
    {
        return lhs.x == rhs.x &&
               lhs.y == rhs.y &&
               lhs.rotation == rhs.rotation;
    }

    template <typename S>
    friend S &operator<<(S &os, const Position &position)
    {
        os << "x: " << position.x << ", y: " << position.y << ", rotation: " << position.rotation;
        return os;
    }
};

template <typename Unit_coordinates, typename Unit_rotation>
struct Position<false, Unit_coordinates, Unit_rotation>
{
    Unit_coordinates forwards{}, sidewards{};
    Unit_rotation rotation{};

    constexpr Position() = default;

    template <typename Unit_coordinates2, typename Unit_rotation2>
    constexpr Position(const Position<false, Unit_coordinates2, Unit_rotation2> &position) : forwards(position.forwards), sidewards(position.sidewards), rotation(position.rotation) {}

    constexpr Position(const Unit_coordinates &forwards, const Unit_coordinates &sidewards, const Unit_rotation &rotation) : forwards(forwards), sidewards(sidewards),
                                                                                                                             rotation(rotation) {}

    constexpr Position(const Vec2d<false, Unit_coordinates> &vec, const Unit_rotation &rotation) : forwards(vec.forwards), sidewards(vec.sidewards),
                                                                                                   rotation(rotation) {}

    [[nodiscard]] constexpr Unit_coordinates total_distance() const
    {
        return sqrt(forwards * forwards + sidewards * sidewards);
    }

    [[nodiscard]] constexpr bool is_zero() const
    {
        return forwards == static_cast<Unit_coordinates>(0) && sidewards == static_cast<Unit_coordinates>(0) && rotation == static_cast<Unit_rotation>(0);
    }

    [[nodiscard]] constexpr Vec2d<false, Unit_coordinates> to_vec() const
    {
        return {forwards, sidewards};
    }

    friend constexpr Position operator-(const Position &lhs, const Position &rhs)
    {
        return {lhs.forwards - rhs.forwards, lhs.sidewards - rhs.sidewards, lhs.rotation - rhs.rotation};
    }

    friend constexpr Position operator-(const Position &lhs, const double &value)
    {
        return {lhs.forwards - value, lhs.sidewards - value, lhs.rotation - value};
    }

    friend constexpr Position operator+(const Position &lhs, const Position &rhs)
    {
        return {lhs.forwards + rhs.forwards, lhs.sidewards + rhs.sidewards, lhs.rotation + rhs.rotation};
    }

    friend constexpr Position operator+(const Position &lhs, const Vec2d<false, Unit_coordinates> &rhs)
    {
        return {lhs.forwards + rhs.forwards, lhs.sidewards + rhs.sidewards, lhs.rotation};
    }

    friend constexpr Position operator+(const Position &lhs, const double &value)
    {
        return {lhs.forwards + value, lhs.sidewards + value, lhs.rotation + value};
    }

    constexpr Position operator+=(const Position &rhs)
    {
        return *this = *this + rhs;
    }

    constexpr Position operator+=(const Vec2d<false, Unit_coordinates> &rhs)
    {
        return *this = *this + rhs;
    }

    friend constexpr Position operator-(const Position &lhs, const Vec2d<false, Unit_coordinates> &rhs)
    {
        return {lhs.forwards - rhs.forwards, lhs.sidewards - rhs.sidewards, lhs.rotation};
    }

    template <typename Unit_coordinates2, typename Unit_rotation2>
    friend constexpr Position<false, typename Multiply<Unit_coordinates, Unit_coordinates2>::Type, typename Multiply<Unit_rotation, Unit_rotation2>::Type> operator*(const Position &lhs, const Position &rhs)
    {
        return {lhs.forwards * rhs.forwards, lhs.sidewards * rhs.sidewards, lhs.rotation * rhs.rotation};
    }

    template <typename Unit>
    friend constexpr Position<false, typename Multiply<Unit_coordinates, Unit>::Type, typename Multiply<Unit_rotation, Unit>::Type> operator*(const Position &lhs, const Unit &value)
    {
        return {lhs.forwards * value, lhs.sidewards * value, lhs.rotation * value};
    }

    template <typename Unit_coordinates2, typename Unit_rotation2>
    friend constexpr Position<false, typename Divide<Unit_coordinates, Unit_coordinates2>::Type, typename Divide<Unit_rotation, Unit_rotation2>::Type> operator/(const Position &lhs, const Position &rhs)
    {
        return {lhs.forwards / rhs.forwards, lhs.sidewards / rhs.sidewards, lhs.rotation / rhs.rotation};
    }

    friend constexpr Position fabs(const Position &lhs)
    {
        return {fabs(lhs.forwards), fabs(lhs.sidewards), fabs(lhs.rotation)};
    }

    friend constexpr Position sgn(const Position &lhs)
    {
        return {(lhs.forwards > 0) - (lhs.forwards < 0), (lhs.sidewards < 0) - (lhs.sidewards < 0), (lhs.rotation < 0) - (lhs.rotation < 0)};
    }

    // Comparison operators need to be in the header
    friend bool operator<(const Position &lhs, const Position &rhs)
    {
        return lhs.forwards < rhs.forwards &&
               lhs.sidewards < rhs.sidewards &&
               lhs.rotation < rhs.rotation;
    }

    friend bool operator==(const Position &lhs, const Position &rhs)
    {
        return lhs.forwards == rhs.forwards &&
               lhs.sidewards == rhs.sidewards &&
               lhs.rotation == rhs.rotation;
    }

    template <typename S>
    friend constexpr S &operator<<(S &os, const Position &position)
    {
        os << "forwards: " << position.forwards << ", sidewards: " << position.sidewards << ", rotation: " << position.rotation;
        return os;
    }
};
