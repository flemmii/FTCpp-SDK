#pragma once

#include "utils.hpp"
#include "navigation/Distance_unit.hpp"

namespace sdk
{
    /**
     * The {@link DistanceSensor} may be found on hardware sensors which measure distance
     * by one means or another.
     */
    class Distance_sensor
    {
    public:
        static jclass jclazz;
        jobject distanceSensor{};

        Distance_sensor() = default;

        Distance_sensor(const jobject &distanceSensor);

        ~Distance_sensor();

        Distance_sensor &operator=(const Distance_sensor &distance_sensor);

        virtual Distance_sensor &operator=(const jobject &distanceSensor);

        /**
         * Returns the current distance in the indicated distance units
         * @param unit  the unit of distance in which the result should be returned
         * @return      the current distance sas measured by the sensor. If no reading is available
         *              (perhaps the sensor is out of range), then {@link #distanceOutOfRange} is
         *              returned;
         */
        double get_distance(Distance_unit unit);

        /**
         * The value returned when a distance reading is not in fact available.
         */
        double distance_out_of_range = std::numeric_limits<double>::infinity();
    };
}
