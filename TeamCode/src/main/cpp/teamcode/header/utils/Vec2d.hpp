#pragma once

#include <string>
#include <cmath>

#include "Units-master/Units.h"

using namespace std;

template <bool field_centric = true, typename Unit = Millimetre>
struct Vec2d;

template <typename Unit>
struct Vec2d<true, Unit>
{
    Unit x{}, y{};

    constexpr Vec2d() = default;

    template <typename Unit2>
    constexpr Vec2d(const Vec2d<true, Unit2> &vec) : x(vec.x), y(vec.y) {}

    constexpr Vec2d(const Unit &x, const Unit &y) : x(x), y(y) {}

    [[nodiscard]] constexpr Unit total_distance() const
    {
        return sqrt(x * x + y * y);
    }

    [[nodiscard]] constexpr bool is_zero() const
    {
        return x == static_cast<Unit>(0) && y == static_cast<Unit>(0);
    }

    friend constexpr Vec2d operator+(const Vec2d &lhs, const Vec2d &rhs)
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    }

    friend constexpr Vec2d operator+(const Vec2d &lhs, const Unit &value)
    {
        return {lhs.x + value, lhs.y + value};
    }

    constexpr Vec2d operator+=(const Vec2d &lhs)
    {
        return *this = *this + lhs;
    }

    friend constexpr Vec2d operator-(const Vec2d &lhs, const Unit &value)
    {
        return {lhs.x - value, lhs.y - value};
    }

    friend constexpr Vec2d operator-(const Vec2d &lhs, const Vec2d &rhs)
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    }

    template <typename Unit2>
    friend constexpr Vec2d<true, typename Multiply<Unit, Unit2>::Type> operator*(const Vec2d &lhs, const Unit2 &value)
    {
        return {lhs.x * value, lhs.y * value};
    }

    template <typename Unit2>
    constexpr Vec2d<true, typename Multiply<Unit, Unit2>::Type> operator*=(const Unit2 &value)
    {
        return *this = *this * value;
    }

    template <typename Unit2>
    constexpr Vec2d<true, typename Divide<Unit, Unit2>::Type> operator/=(const Unit2 &value)
    {
        return *this = *this / value;
    }

    template <typename Unit2>
    friend constexpr Vec2d<true, typename Multiply<Unit, Unit2>::Type> operator*(const Vec2d &lhs, const Vec2d<true, Unit2> &rhs)
    {
        return {lhs.x * rhs.x, lhs.y * rhs.y};
    }

    template <typename Unit2>
    friend constexpr Vec2d<true, typename Divide<Unit, Unit2>::Type> operator/(const Vec2d &lhs, const Vec2d<true, Unit2> &rhs)
    {
        return {lhs.x / rhs.x, lhs.y / rhs.y};
    }

    template <typename Unit2>
    friend constexpr Vec2d<true, typename Divide<Unit, Unit2>::Type> operator/(const Vec2d &lhs, const Unit2 &value)
    {
        return {lhs.x / value, lhs.y / value};
    }

    friend constexpr Vec2d sgn(const Vec2d &lhs)
    {
        return {(lhs.x > 0) - (lhs.x < 0), (lhs.y < 0) - (lhs.y < 0)};
    }

    friend constexpr Vec2d abs(const Vec2d &lhs)
    {
        return {abs(lhs.x), abs(lhs.y)};
    }

    // Comparison operators need to be in the header
    friend bool operator<(const Vec2d &lhs, const Vec2d &rhs)
    {
        return lhs.x < rhs.x &&
               lhs.y < rhs.y;
    }

    friend bool operator==(const Vec2d &lhs, const Vec2d &rhs)
    {
        return lhs.x == rhs.x &&
               lhs.y == rhs.y;
    }

    template <typename S>
    friend S &operator<<(S &os, const Vec2d &vec)
    {
        os << "x: " << vec.x << ", y: " << vec.y;
        return os;
    }
};

template <typename Unit>
struct Vec2d<false, Unit>
{
    Unit forwards{}, sidewards{};

    constexpr Vec2d() = default;

    template <typename Unit2>
    constexpr Vec2d(const Vec2d<false, Unit2> &vec) : forwards(vec.forwards), sidewards(vec.sidewards) {}

    constexpr Vec2d(const Unit &forwards, const Unit &sidewards) : forwards(forwards),
                                                                   sidewards(sidewards) {}

    [[nodiscard]] constexpr Unit total_distance() const
    {
        return sqrt(forwards * forwards + sidewards * sidewards);
    }

    [[nodiscard]] constexpr bool is_zero() const
    {
        return forwards == static_cast<Unit>(0) && sidewards == static_cast<Unit>(0);
    }

    friend constexpr Vec2d operator+(const Vec2d &lhs, const Vec2d &rhs)
    {
        return {lhs.forwards + rhs.forwards, lhs.sidewards + rhs.sidewards};
    }

    friend constexpr Vec2d operator+(const Vec2d &lhs, const Unit &value)
    {
        return {lhs.forwards + value, lhs.sidewards + value};
    }

    friend constexpr Vec2d operator-(const Vec2d &lhs, const Unit &value)
    {
        return {lhs.forwards - value, lhs.sidewards - value};
    }

    friend constexpr Vec2d operator-(const Vec2d &lhs, const Vec2d &rhs)
    {
        return {lhs.forwards - rhs.forwards, lhs.sidewards - rhs.sidewards};
    }

    template <typename Unit2>
    friend constexpr Vec2d<false, typename Multiply<Unit, Unit2>::Type> operator*(const Vec2d &lhs, const Unit2 &value)
    {
        return {lhs.forwards * value, lhs.sidewards * value};
    }

    template <typename Unit2>
    constexpr Vec2d<false, typename Multiply<Unit, Unit2>::Type> operator*=(const Unit2 &value)
    {
        return *this = *this * value;
    }

    template <typename Unit2>
    friend constexpr Vec2d<false, typename Multiply<Unit, Unit2>::Type> operator*(const Vec2d &lhs, const Vec2d &rhs)
    {
        return {lhs.forwards * rhs.forwards, lhs.sidewards * rhs.sidewards};
    }

    template <typename Unit2>
    friend constexpr Vec2d<false, typename Divide<Unit, Unit2>::Type> operator/(const Vec2d &lhs, const Vec2d &rhs)
    {
        return {lhs.forwards / rhs.forwards, lhs.sidewards / rhs.sidewards};
    }

    friend constexpr Vec2d sgn(const Vec2d &lhs)
    {
        return {(lhs.forwards > 0) - (lhs.forwards < 0), (lhs.sidewards < 0) - (lhs.sidewards < 0)};
    }

    friend constexpr Vec2d abs(const Vec2d &lhs)
    {
        return {abs(lhs.forwards), abs(lhs.sidewards)};
    }

    // Comparison operators need to be in the header
    friend bool operator<(const Vec2d &lhs, const Vec2d &rhs)
    {
        return lhs.forwards < rhs.forwards &&
               lhs.sidewards < rhs.sidewards;
    }

    friend bool operator==(const Vec2d &lhs, const Vec2d &rhs)
    {
        return lhs.forwards == rhs.forwards &&
               lhs.sidewards == rhs.sidewards;
    }

    template <typename S>
    friend S &operator<<(S &os, const Vec2d &vec)
    {
        os << "forwards: " << vec.forwards << ", sidewards: " << vec.sidewards;
        return os;
    }
};