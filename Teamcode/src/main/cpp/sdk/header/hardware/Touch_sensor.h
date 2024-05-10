//
// Created by fnlg on 10.05.2024.
//

#ifndef FTCROBOTCONTROLLER_TOUCH_SENSOR_H
#define FTCROBOTCONTROLLER_TOUCH_SENSOR_H

#include "extras/utils.h"

namespace sdk {
    extern jclass TouchSensor;

    class Touch_sensor {
    public:
        jobject touchSensor{};

        Touch_sensor() = default;

        Touch_sensor(jobject touchSensor);

        ~Touch_sensor();

        [[nodiscard]] double get_value() const;

        [[nodiscard]] bool is_pressed() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_TOUCH_SENSOR_H
