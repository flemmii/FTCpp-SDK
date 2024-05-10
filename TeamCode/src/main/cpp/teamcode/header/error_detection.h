#ifndef ERROR_DETECTION_H
#define ERROR_DETECTION_H

#include <map>
#include <string>

#include "robot.h"
#include "../sdk/header/telemetry.h"

namespace error_detection {
    bool detect_fire(const std::string &motor_name, double current_value, double target_value,
                     int max_wrong_current_values = 10);

    bool is_my_motor_on_fire(const std::string &motorName);

    std::map<std::string, bool> get_motors_on_fire();

    void print_motor_status();
}

#endif // ERROR_DETECTION_H
