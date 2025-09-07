#pragma once

#include <limits>
#include <string>
#include <cstdio>

namespace sdk
{
    struct Distance_unit
    {
        static jclass jclazz;

        static constexpr double mm_per_inch = 25.4;
        static constexpr double m_per_inch = mm_per_inch * 0.001;

        static const Distance_unit METER;
        static const Distance_unit CM;
        static const Distance_unit MM;
        static const Distance_unit INCH;

        const int value;

        constexpr Distance_unit(int value) : value(value) {}

        // Assignment operator
        constexpr Distance_unit &operator=(const Distance_unit &other)
        {
            if (this != &other)
            {
                const_cast<int &>(this->value) = other.value;
            }
            return *this;
        }

        //----------------------------------------------------------------------------------------------
        // Primitive operations
        //----------------------------------------------------------------------------------------------

        constexpr double from_meters(double meters) const
        {
            if (meters == std::numeric_limits<double>::max())
                return std::numeric_limits<double>::max();
            switch (value)
            {
            default:
            case 0:
                return meters;
            case 1:
                return meters * 100;
            case 2:
                return meters * 1000;
            case 3:
                return meters / m_per_inch;
            }
        }

        constexpr double from_inches(double inches) const
        {
            if (inches == std::numeric_limits<double>::max())
                return std::numeric_limits<double>::max();
            switch (value)
            {
            default:
            case 0:
                return inches * m_per_inch;
            case 1:
                return inches * m_per_inch * 100;
            case 2:
                return inches * m_per_inch * 1000;
            case 3:
                return inches;
            }
        }

        constexpr double from_cm(double cm) const
        {
            if (cm == std::numeric_limits<double>::max())
                return std::numeric_limits<double>::max();
            switch (value)
            {
            default:
            case 0:
                return cm / 100;
            case 1:
                return cm;
            case 2:
                return cm * 10;
            case 3:
                return from_meters(METER.from_cm(cm));
            }
        }

        constexpr double from_mm(double mm) const
        {
            if (mm == std::numeric_limits<double>::max())
                return std::numeric_limits<double>::max();
            switch (value)
            {
            default:
            case 0:
                return mm / 1000;
            case 1:
                return mm / 10;
            case 2:
                return mm;
            case 3:
                return from_meters(METER.from_mm(mm));
            }
        }

        constexpr double from_unit(const Distance_unit &him, double his) const
        {
            switch (him.value)
            {
            default:
            case 0:
                return from_meters(his);
            case 1:
                return from_cm(his);
            case 2:
                return from_mm(his);
            case 3:
                return from_inches(his);
            }
        }

        //----------------------------------------------------------------------------------------------
        // Derived operations
        //----------------------------------------------------------------------------------------------

        constexpr double to_meters(double in_our_units) const
        {
            switch (value)
            {
            default:
            case 0:
                return METER.from_meters(in_our_units);
            case 1:
                return METER.from_cm(in_our_units);
            case 2:
                return METER.from_mm(in_our_units);
            case 3:
                return METER.from_inches(in_our_units);
            }
        }

        constexpr double to_inches(double in_our_units) const
        {
            switch (value)
            {
            default:
            case 0:
                return INCH.from_meters(in_our_units);
            case 1:
                return INCH.from_cm(in_our_units);
            case 2:
                return INCH.from_mm(in_our_units);
            case 3:
                return INCH.from_inches(in_our_units);
            }
        }

        constexpr double to_cm(double in_our_units) const
        {
            switch (value)
            {
            default:
            case 0:
                return CM.from_meters(in_our_units);
            case 1:
                return CM.from_cm(in_our_units);
            case 2:
                return CM.from_mm(in_our_units);
            case 3:
                return CM.from_inches(in_our_units);
            }
        }

        constexpr double to_mm(double in_our_units) const
        {
            switch (value)
            {
            default:
            case 0:
                return MM.from_meters(in_our_units);
            case 1:
                return MM.from_cm(in_our_units);
            case 2:
                return MM.from_mm(in_our_units);
            case 3:
                return MM.from_inches(in_our_units);
            }
        }

        //----------------------------------------------------------------------------------------------
        // Formatting
        //----------------------------------------------------------------------------------------------

        std::string to_string(double in_our_units) const
        {
            char buffer[50];
            switch (value)
            {
            default:
            case 0:
                snprintf(buffer, sizeof(buffer), "%.3fm", in_our_units);
                break;
            case 1:
                snprintf(buffer, sizeof(buffer), "%.1fcm", in_our_units);
                break;
            case 2:
                snprintf(buffer, sizeof(buffer), "%.0fmm", in_our_units);
                break;
            case 3:
                snprintf(buffer, sizeof(buffer), "%.2fin", in_our_units);
                break;
            }
            return std::string(buffer);
        }

        std::string to_string() const
        {
            switch (value)
            {
            default:
            case 0:
                return "m";
            case 1:
                return "cm";
            case 2:
                return "mm";
            case 3:
                return "in";
            }
        }
    };

    constexpr Distance_unit Distance_unit::METER{0};
    constexpr Distance_unit Distance_unit::CM{1};
    constexpr Distance_unit Distance_unit::MM{2};
    constexpr Distance_unit Distance_unit::INCH{3};

} // namespace sdk