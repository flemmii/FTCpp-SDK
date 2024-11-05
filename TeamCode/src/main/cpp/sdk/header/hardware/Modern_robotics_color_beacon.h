//
// Created by flemmi on 05.11.24.
//

#ifndef FTCROBOTCONTROLLER_MODERN_ROBOTICS_COLOR_BEACON_H
#define FTCROBOTCONTROLLER_MODERN_ROBOTICS_COLOR_BEACON_H

#include <string>

#include "utils.h"

namespace sdk {
    class Modern_robotics_color_beacon {
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

        void set_color(int red, int green, int blue) const;

        bool do_initialize() const;

        [[nodiscard]] std::string get_device_name() const;
    };
}

#endif //FTCROBOTCONTROLLER_MODERN_ROBOTICS_COLOR_BEACON_H
