#pragma once

#include <cmath>
#include <type_traits>

#include "utils/Position.hpp"

template <typename Unit_coordinates = Millimetre, typename Unit_rotation = Degree>
struct Location
{
    Unit_coordinates coordinates{};
    Unit_rotation rotation{};

    constexpr Location() = default;

    template <typename Unit_coordinates2, typename Unit_rotation2>
    constexpr Location(const Location<Unit_coordinates2, Unit_rotation2> &location) : coordinates(location.coordinates),
                                                                                      rotation(location.rotation) {}

    constexpr Location(const Unit_coordinates &coordinates, const Unit_rotation &rotation) : coordinates(coordinates),
                                                                                             rotation(rotation) {}

    constexpr explicit Location(const Position<true, Unit_coordinates, Unit_rotation> position) : coordinates(position.total_distance()),
                                                                                                  rotation(position.rotation) {}

    constexpr explicit Location(const Position<false, Unit_coordinates, Unit_rotation> &position) : coordinates(position.total_distance()),
                                                                                                    rotation(position.rotation) {}

    [[nodiscard]] constexpr bool is_zero() const
    {
        return coordinates == static_cast<Unit_coordinates>(0) && rotation == static_cast<Unit_rotation>(0);
    }

    template <typename T, typename R>
    constexpr typename std::enable_if<std::is_floating_point<T>::value && std::is_floating_point<R>::value, Location<T, R>>::type
    fabs(const Location<T, R> &location)
    {
        return {std::fabs(location.coordinates), std::fabs(location.rotation)};
    }

    template <typename T, typename R>
    friend constexpr Location abs(const Location<T, R> &location)
    {
        return {abs(location.coordinates), abs(location.rotation)};
    }

    template <typename T, typename R>
    typename std::enable_if<(std::is_arithmetic<T>::value && std::is_arithmetic<R>::value) || (units::IsUnit<T>::Value && units::IsUnit<R>::Value), Location<T, R>>::type constexpr sgn(const Location<T, R> &location) const
    {
        return {(location.coordinates > 0) - (location.coordinates < 0), (location.rotation > 0) - (location.rotation < 0)};
    }

    template <typename T, typename R>
    typename std::enable_if<(std::is_arithmetic<T>::value && std::is_arithmetic<R>::value) || (units::IsUnit<T>::Value && units::IsUnit<R>::Value), Location<T, R>>::type constexpr operator+(const Location<T, R> &other) const
    {
        return {coordinates + other.coordinates, rotation + other.rotation};
    }

    template <typename T, typename R>
    typename std::enable_if<(std::is_arithmetic<T>::value && std::is_arithmetic<R>::value) || (units::IsUnit<T>::Value && units::IsUnit<R>::Value), Location<T, R>>::type constexpr operator*(const double &value) const
    {
        return {coordinates * value, rotation * value};
    }

    template <typename T, typename R>
    typename std::enable_if<(std::is_arithmetic<T>::value && std::is_arithmetic<R>::value) || (units::IsUnit<T>::Value && units::IsUnit<R>::Value), Location<T, R>>::type constexpr operator*(const Location<T, R> &other) const
    {
        return {coordinates * other.coordinates, rotation * other.rotation};
    }

    template <typename T, typename R>
    typename std::enable_if<(std::is_arithmetic<T>::value && std::is_arithmetic<R>::value) || (units::IsUnit<T>::Value && units::IsUnit<R>::Value), Location<T, R>>::type constexpr operator/(const Location<T, R> &other) const
    {
        return {coordinates / other.coordinates, rotation / other.rotation};
    }

    template <typename S>
    friend S &operator<<(S &os, const Location &loc)
    {
        os << "coordinates: " << loc.coordinates << ", rotation: " << loc.rotation;
        return os;
    }

    friend bool operator<(const Location &lhs, const Location &rhs)
    {
        return lhs.coordinates < rhs.coordinates &&
               lhs.rotation < rhs.rotation;
    }
};