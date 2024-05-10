//
// Created by fnlg on 03.02.2024.
//

#include "hardware/Gamepad/Analog_sticks.h"

namespace sdk {
    void Analog_sticks::add_update_listener(
            const std::function<void(float, float, float, float)> &listener) {
        update_listener_vector.push_back(listener);
    }

    void Analog_sticks::remove_update_listener(Analog_sticks::Update_listener listener) {
        update_listener_vector.erase(
                std::remove_if(update_listener_vector.begin(), update_listener_vector.end(),
                               [&](const Update_listener &elem) { return &elem == &listener; }),
                update_listener_vector.end());
    }

    void Analog_sticks::clear_update_listener_list() {
        update_listener_vector.clear();
    }

    void Analog_sticks::update(float left_x, float left_y, float right_x, float right_y) {
        for (const auto &listener: update_listener_vector) {
            listener(left_x, left_y, right_x, right_y);
        }
    }
} // sdk