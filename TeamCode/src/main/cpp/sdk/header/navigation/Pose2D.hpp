#pragma once

#include "navigation/Angle_unit.hpp"
#include "navigation/Distance_unit.hpp"

namespace sdk
{
    /**
     * Pose2D represents the position and heading of an object in 2D space.
     */
    struct Pose2D
    {
    protected:
        double x;
        double y;
        sdk::Distance_unit distance_unit;
        double heading;
        sdk::Angle_unit heading_unit;

    public:
        static jclass jclazz;
        /**
         * Creates a new Pose2D object.
         * @param distance_unit the unit of distance for both x and y
         * @param x the x position
         * @param y the y position
         * @param heading_unit the unit of heading
         * @param heading the heading
         */
        constexpr Pose2D(Distance_unit distance_unit, double x, double y, Angle_unit heading_unit, double heading)
            : x(x), y(y), distance_unit(distance_unit.value), heading(heading), heading_unit(heading_unit) {}

        /**
         * This gets X in the desired distance unit
         * @param unit the desired distance unit
         * @return the X member converted to the desired distance unit
         */
        constexpr double getX(Distance_unit unit) const
        {
            return unit.from_unit(distance_unit.value, x);
        }

        /**
         * This gets the Y in the desired distance unit
         * @param unit the desired distance unit
         * @return the Y member converted to the desired distance unit
         */
        constexpr double getY(Distance_unit unit) const
        {
            return unit.from_unit(distance_unit.value, y);
        }

        /**
         * This gets the heading in the desired distance unit
         * Be aware that this normalizes the angle to be between -PI and PI for RADIANS or
         * between -180 and 180 for DEGREES
         * @param unit the desired distance unit
         * @return the heading converted to the desired Angle Unit
         */
        constexpr double getHeading(Angle_unit unit) const
        {
            return unit.from_unit(this->heading_unit, heading);
        }
    };

} // namespace sdk
