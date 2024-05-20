#ifndef FTC_FROG_DISTANCE_SENSOR_H
#define FTC_FROG_DISTANCE_SENSOR_H

#include "utils.h"

namespace sdk {
    extern jclass DistanceSensor;
    extern jclass DistanceUnit;

    class Distance_sensor {
    private:
        jobject distanceSensor{};
    public:
        Distance_sensor() = default; // Constructor declaration

        Distance_sensor(jobject distanceSensor);

        // Returns the distance in mm
        double get_distance();
    };
}

#endif // FTC_FROG_DISTANCE_SENSOR_H
