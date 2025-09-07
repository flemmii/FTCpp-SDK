#pragma once

#include "utils/Vec3d.hpp"
#include "Units-master/Units.h"

namespace field
{
    struct Object
    {
        const Vec3d<Millimetre> bottom_left;
        const Vec3d<Millimetre> top_right;

        constexpr Object(const Vec3d<Millimetre> &bottom_left, const Vec3d<Millimetre> &top_right) : bottom_left(
                                                                                                         bottom_left),
                                                                                                     top_right(top_right) {}

        [[nodiscard]] constexpr Vec3d<Millimetre> center() const
        {
            return (bottom_left + top_right) / 2;
        }
    };

    // field tile with nubs on one side
    constexpr Object field_tiles({0_mm, 0_mm, -15875_um},
                                 {600_mm, 600_mm, 0_mm});
    // 6 * field tiles but with the distance of one nub subtracted
    constexpr Object field({0_mm, 0_mm, -15875_um},
                           {3580_mm, 3580_mm, 0_mm});

    // Alliance specific samples
    namespace samples
    {
        namespace alliance_specific
        {
            namespace own_alliance
            {
                constexpr Object field({2993_mm, 1091100_um, 0_mm},
                                       {3031_mm, 1180_mm, 38100_um});
                constexpr Object middle({3247_mm, 1091100_um, 0_mm},
                                        {3285_mm, 1180_mm, 38100_um});
                constexpr Object wall({3501_mm, 1091100_um, 0_mm},
                                      {3539_mm, 1180_mm, 38100_um});
            }

            namespace opposite_alliance
            {
                constexpr Object field({587_mm, 2400_mm, 0_mm},
                                       {625_mm, 2488900_um, 38100_um});
                constexpr Object middle({333_mm, 2400_mm, 0_mm},
                                        {371_mm, 2488900_um, 38100_um});
                constexpr Object wall({79_mm, 2400_mm, 0_mm},
                                      {117_mm, 2488900_um, 38100_um});
            }
        }

        namespace neutral
        {
            namespace own_alliance
            {
                constexpr Object field({587_mm, 1091100_um, 0_mm},
                                       {625_mm, 1180_mm, 38100_um});
                constexpr Object middle({333_mm, 1091100_um, 0_mm},
                                        {371_mm, 1180_mm, 38100_um});
                constexpr Object wall({79_mm, 1091100_um, 0_mm},
                                      {117_mm, 1180_mm, 38100_um});
            }

            namespace opposite_alliance
            {
                constexpr Object field({2993_mm, 2400_mm, 0_mm},
                                       {3031_mm, 2488900_um, 38100_um});
                constexpr Object middle({3247_mm, 2400_mm, 0_mm},
                                        {3285_mm, 2488900_um, 38100_um});
                constexpr Object wall({3501_mm, 2400_mm, 0_mm},
                                      {3539_mm, 2488900_um, 38100_um});
            }
        }
    }

    namespace submersible
    {
        namespace side_support_structure
        {
            constexpr Object top_right({2167190_um, 2355150_um, 0_mm},
                                       {2402140_um, 2402775_um, 50800_um});

            constexpr Object top_left({1177860_um, 2355150_um, 0_mm},
                                      {1412810_um, 2402775_um, 50800_um});

            constexpr Object bottom_right({2167190_um, 1177225_um, 0_mm},
                                          {2402140_um, 1224850_um, 50800_um});

            constexpr Object bottom_left({1177860_um, 1177225_um, 0_mm},
                                         {1412810_um, 1222850_um, 50800_um});
        }

        namespace rods
        {
            constexpr Object top_right({2088450_um, 2355150_um, 0_mm},
                                       {2164650_um, 2402775_um, 914400_um});

            constexpr Object top_left({1415350_um, 2355150_um, 0_mm},
                                      {1491650_um, 2402775_um, 914400_um});

            constexpr Object bottom_right({2088450_um, 1177225_um, 0_mm},
                                          {2164650_um, 1224850_um, 914400_um});

            constexpr Object bottom_left({1415350_um, 1177225_um, 0_mm},
                                         {1491650_um, 1224850_um, 914400_um});
        }

        namespace upper_crossbeams
        {
            constexpr Object top({2332925_um, 1415350_um, 841348_um},
                                 {2355150_um, 2164650_um, 889_mm});

            constexpr Object bottom({1224850_um, 1415350_um, 841348_um},
                                    {1247075_um, 2164650_um, 889_mm});
        }

        namespace barrier
        {
            constexpr Object top({1453450_um, 2355150_um, 0_mm},
                                 {2126650_um, 2402775_um, 50800_um});

            constexpr Object bottom({1453450_um, 1177225_um, 0_mm},
                                    {2126650_um, 1224850_um, 50800_um});

            constexpr Object right({2088450_um, 1224850_um, 25400_um},
                                   {2164650_um, 2355150_um, 50800_um});

            constexpr Object left({1415350_um, 1224850_um, 25400_um},
                                  {1491650_um, 2355150_um, 50800_um});
        }

        namespace chamber
        {
            namespace own_alliance
            {
                constexpr Object low({1453450_um, 1177225_um, 303530_um},
                                     {2126650_um, 1203895_um, 330200_um});

                constexpr Object high({1453450_um, 1177225_um, 633730_um},
                                      {2126650_um, 1203895_um, 660400_um});
            }

            namespace opposite_alliance
            {
                constexpr Object low({1453450_um, 2376105_um, 303530_um},
                                     {2126650_um, 2402775_um, 330200_um});

                constexpr Object high({1453450_um, 2376105_um, 633730_um},
                                      {2126650_um, 2402775_um, 660400_um});
            }
        }

        namespace rungs
        {
            namespace own_alliance
            {
                constexpr Object low({1415350_um, 1224850_um, 482600_um},
                                     {1440750_um, 2355150_um, 508_mm});

                constexpr Object high({1415350_um, 1224850_um, 889_mm},
                                      {1440750_um, 2355150_um, 914400_um});
            }

            namespace opposite_alliance
            {
                constexpr Object low({2139250_um, 1224850_um, 482600_um},
                                     {2164650_um, 2355150_um, 508_mm});

                constexpr Object high({2139250_um, 1224850_um, 889_mm},
                                      {2164650_um, 2355150_um, 914400_um});
            }
        }
    }
}