#pragma once

#include <string>

#include "utils.hpp"

namespace sdk
{
    class Modern_robotics_color_beacon
    {
    public:
        static jclass jclazz;
        jobject modernRoboticsColorBeacon{};

        Modern_robotics_color_beacon() = default;

        Modern_robotics_color_beacon(const jobject &modernRoboticsColorBeacon);

        ~Modern_robotics_color_beacon();

        Modern_robotics_color_beacon &
        operator=(const Modern_robotics_color_beacon &modern_robotics_color_beacon);

        Modern_robotics_color_beacon &operator=(const jobject &modernRoboticsColorBeacon);

        void turn_LEDs_off() const;

        void set_red() const;

        void set_green() const;

        void set_yellow() const;

        void set_blue() const;

        void set_purple() const;

        void set_teal() const;

        void set_white() const;

        void set_color(const int &red, const int &green, const int &blue) const;

        bool do_initialize() const;

        [[nodiscard]] std::string get_device_name() const;
    };
}
