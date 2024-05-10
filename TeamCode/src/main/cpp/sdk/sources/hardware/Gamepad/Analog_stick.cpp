//
// Created by fnlg on 03.02.2024.
//

#include "hardware/Gamepad/Analog_stick.h"


namespace sdk {
    void Analog_stick::add_update_listener(const Update_listener &listener) {
        update_listener_vector.push_back(listener);
    }

    void Analog_stick::remove_update_listener(Analog_stick::Update_listener listener) {
        update_listener_vector.erase(
                remove_if(update_listener_vector.begin(), update_listener_vector.end(),
                          [&](const Update_listener &elem) { return &elem == &listener; }),
                update_listener_vector.end());
    }

    void Analog_stick::clear_update_listener_list() {
        update_listener_vector.clear();
    }

    void Analog_stick::update(float x, float y) {
        for (const auto &listener: update_listener_vector) {
            listener(x, y);
        }
    }
}
