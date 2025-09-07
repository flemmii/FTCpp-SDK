#pragma once

#include <string>
#include "Units-master/Units.h"

using namespace std;

template <typename Unit = Millimetre>
struct Vec3d
{
    Unit x{}, y{}, z{};

    constexpr Vec3d(const Unit &x, const Unit &y, const Unit &z) : x(x), y(y), z(z) {}

    constexpr typename std::enable_if<units::IsUnit<Unit>::Value || std::is_arithmetic<Unit>::value, Vec3d<Unit>>::type operator+(const Vec3d &other) const
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    constexpr typename std::enable_if<units::IsUnit<Unit>::Value || std::is_arithmetic<Unit>::value, Vec3d<Unit>>::type operator/(const double &other) const
    {
        return {x / other, y / other, z / other};
    }

    template <typename S>
    friend constexpr S &operator<<(S &os, const Vec3d &vec)
    {
        os << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
        return os;
    }
};
