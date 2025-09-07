#pragma once

#include <string>

#include "utils.hpp"

namespace sdk
{
    class Gyro_sensor
    {
    public:
        static jclass jclazz;
        jobject gyroSensor{};

        Gyro_sensor() = default;

        Gyro_sensor(const jobject &gyroSensor);

        ~Gyro_sensor();

        Gyro_sensor &operator=(const Gyro_sensor &gyro_sensor);

        Gyro_sensor &operator=(const jobject &gyroSensor);

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
