#pragma once

#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

#include "Angle_unit.hpp"

namespace sdk
{

    struct Yaw_pitch_roll_angles
    {
    private:
        double yaw_degrees;
        double pitch_degrees;
        double roll_degrees;
        const long acquisition_time;

    public:
        Yaw_pitch_roll_angles(const Angle_unit &angle_unit, const double &yaw, const double &pitch,
                              const double &roll, const long &acquisition_time);

        double get_yaw(const Angle_unit &angle_unit) const;

        double get_pitch(const Angle_unit &angle_unit) const;

        double get_roll(const Angle_unit &angle_unit) const;

        long get_acquisition_time() const;

        std::string to_string() const;
    };

} // sdk
