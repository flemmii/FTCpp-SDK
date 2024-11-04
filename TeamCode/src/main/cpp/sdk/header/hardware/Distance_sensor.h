#ifndef FTC_FROG_DISTANCE_SENSOR_H
#define FTC_FROG_DISTANCE_SENSOR_H

#include "utils.h"

namespace sdk {

    class Distance_sensor {
    public:
        static jclass jclazz;
        static jclass DistanceUnit;
        jobject distanceSensor{};

        Distance_sensor() = default;

        Distance_sensor(const jobject &distanceSensor);

        ~Distance_sensor();

        Distance_sensor &operator=(const Distance_sensor &distance_sensor);

        virtual Distance_sensor &operator=(const jobject &distanceSensor);

        // TODO: Add Distance_unit
        // Returns the distance in mm
        [[nodiscard]] double get_distance() const;
    };
}

#endif // FTC_FROG_DISTANCE_SENSOR_H
