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

        Distance_sensor(jobject distanceSensor);

        ~Distance_sensor();

        Distance_sensor &operator=(jobject distanceSensor);

        // TODO: Add Distance_unit
        // Returns the distance in mm
        double get_distance();
    };
}

#endif // FTC_FROG_DISTANCE_SENSOR_H
