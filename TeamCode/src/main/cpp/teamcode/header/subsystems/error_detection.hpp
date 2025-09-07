#pragma once

#include <chrono>

#include "Units-master/Units.h"

namespace error_detection
{
    size_t add_motor(const std::string &motor_name, int max_wrong_values = 10,
                     std::chrono::steady_clock::duration cooldown = std::chrono::milliseconds(10));

    bool detect_fire(size_t index, Tick current_value, Power target_value);

    bool is_my_motor_on_fire(size_t index);

    std::vector<std::string> get_motors_on_fire();
}