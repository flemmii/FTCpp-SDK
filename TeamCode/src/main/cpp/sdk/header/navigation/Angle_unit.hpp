#pragma once

#include <jni.h>
#include <cmath>

namespace sdk
{
    /**
     * An {@link AngleUnit} represents angles in different units of measure and
     * provides utility methods to convert across units. {@link AngleUnit} does not
     * maintain angle information information internally, but only helps organize
     * and use angle measures that may be maintained separately across various contexts.
     * <p>
     * Angles can be distinguished along (at least) two axes:
     *  <ol>
     *      <li>the fundamental unit (radians vs degrees)</li>
     *      <li>whether the angular quantity is normalized or not to the range of [-180,+180) degrees</li>
     *  </ol>
     *  Normalized angles are of most utility when dealing with physical angles, as normalization
     *  removes ambiguity of representation. In particular, two angles can be compared for equality
     *  by subtracting them, normalizing, and testing whether the absolute value of the result is
     *  smaller than some tolerance threshold. This approach neatly handles all cases of cyclical
     *  wrapping without unexpected discontinuities.
     * <p>
     *  Unnormalized angles can be handy when the angular quantity is not a physical angle but some
     *  related quantity such as an angular <em>velocity</em> or <em>acceleration</em>, where the
     *  quantity in question lacks the 360-degree cyclical equivalence of a physical angle.
     * <p>
     *  {@link AngleUnit} expresses normalized angles, while {@link UnnormalizedAngleUnit} expresses unnormalized ones
     * </p>
     */
    struct Angle_unit
    {
        static jclass jclazz;

        static constexpr double two_pi = 2 * M_PI;
        static constexpr float pif = static_cast<float>(M_PI);

        static const Angle_unit DEGREES;
        static const Angle_unit RADIANS;

        const int value;

        constexpr Angle_unit(int value) : value(value) {}

        // Assignment operator
        constexpr Angle_unit &operator=(const Angle_unit &other)
        {
            if (this != &other)
            {
                const_cast<int &>(this->value) = other.value;
            }
            return *this;
        }

        constexpr bool operator==(const Angle_unit &other) const
        {
            return value == other.value;
        }

        constexpr double from_degrees(double degrees) const
        {
            return (value == 1) ? normalize(degrees / 180.0 * M_PI) : normalize(degrees);
        }

        constexpr float from_degrees(float degrees) const
        {
            return (value == 1) ? normalize(degrees / 180.0f * pif) : normalize(degrees);
        }

        constexpr double from_radians(double radians) const
        {
            return (value == 1) ? normalize(radians) : normalize(radians / M_PI * 180.0);
        }

        constexpr float from_radians(float radians) const
        {
            return (value == 1) ? normalize(radians) : normalize(radians / pif * 180.0f);
        }

        constexpr double from_unit(const Angle_unit &them, double theirs) const
        {
            return (them.value == 1) ? from_radians(theirs) : from_degrees(theirs);
        }

        constexpr float from_unit(const Angle_unit &them, float theirs) const
        {
            return (them.value == 1) ? from_radians(theirs) : from_degrees(theirs);
        }

        constexpr double to_degrees(double in_our_units) const
        {
            return (value == 1) ? DEGREES.from_radians(in_our_units) : DEGREES.from_degrees(in_our_units);
        }

        constexpr float to_degrees(float in_our_units) const
        {
            return (value == 1) ? DEGREES.from_radians(in_our_units) : DEGREES.from_degrees(in_our_units);
        }

        constexpr double to_radians(double in_our_units) const
        {
            return (value == 1) ? RADIANS.from_radians(in_our_units) : RADIANS.from_degrees(in_our_units);
        }

        constexpr float to_radians(float in_our_units) const
        {
            return (value == 1) ? RADIANS.from_radians(in_our_units) : RADIANS.from_degrees(in_our_units);
        }

        constexpr double normalize(double mine) const
        {
            return (value == 1) ? normalize_radians(mine) : normalize_degrees(mine);
        }

        constexpr float normalize(float mine) const
        {
            return (value == 1) ? normalize_radians(mine) : normalize_degrees(mine);
        }

        static constexpr double normalize_degrees(double degrees)
        {
            while (degrees >= 180.0)
                degrees -= 360.0;
            while (degrees < -180.0)
                degrees += 360.0;
            return degrees;
        }

        static constexpr float normalize_degrees(float degrees)
        {
            return static_cast<float>(normalize_degrees(static_cast<double>(degrees)));
        }

        static constexpr double normalize_radians(double radians)
        {
            while (radians >= M_PI)
                radians -= two_pi;
            while (radians < -M_PI)
                radians += two_pi;
            return radians;
        }

        static constexpr float normalize_radians(float radians)
        {
            return static_cast<float>(normalize_radians(static_cast<double>(radians)));
        }
    };

    constexpr Angle_unit Angle_unit::DEGREES{0};
    constexpr Angle_unit Angle_unit::RADIANS{1};

} // namespace sdk
