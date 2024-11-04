//
// Created by flemmi on 04.11.24.
//

#include "hardware/Color_range_sensor.h"

namespace sdk {
    jclass Color_range_sensor::jclazz;

    Color_range_sensor::Color_range_sensor(const jobject &colorRangeSensor) : Color_sensor(
            colorRangeSensor), Distance_sensor(colorRangeSensor), Light_sensor(colorRangeSensor) {}

    Color_range_sensor &Color_range_sensor::operator=(const jobject &colorRangeSensor) {
        Color_sensor::operator=(colorRangeSensor);
        Distance_sensor::operator=(colorRangeSensor);
        Light_sensor::operator=(colorRangeSensor);
        return *this;
    }
} // sdk