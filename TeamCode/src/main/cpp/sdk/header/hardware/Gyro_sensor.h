//
// Created by fnlg on 10.05.2024.
//

#ifndef FTCROBOTCONTROLLER_GYRO_SENSOR_H
#define FTCROBOTCONTROLLER_GYRO_SENSOR_H

#include <string>

#include "extras/utils.h"

namespace sdk {
    extern jclass GyroSensor;

    class Gyro_sensor {
    public:
        jobject gyroSensor{};

        Gyro_sensor() = default;

        Gyro_sensor(jobject gyroSensor);

        void calibrate() const;

        [[nodiscard]] bool is_calibrating() const;

        [[nodiscard]] int get_heading() const;

        [[nodiscard]] double get_rotation_fraction() const;

        [[nodiscard]] int raw_x() const;

        [[nodiscard]] int raw_y() const;

        [[nodiscard]] int raw_z() const;

        void reset_z_axis_integrator() const;

        [[nodiscard]] std::string status() const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_GYRO_SENSOR_H
