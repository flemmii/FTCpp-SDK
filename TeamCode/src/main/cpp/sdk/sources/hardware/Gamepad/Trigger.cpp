//
// Created by fnlg on 03.02.2024.
//

#include "hardware/Gamepad/Trigger.h"

namespace sdk {
    void Trigger::add_full_press_listener(const Trigger::Trigger_listener &listener) {
        trigger_full_press_listener_vector.push_back(listener);
    }

    void Trigger::remove_full_press_listener(Trigger::Trigger_listener listener) {
        trigger_full_press_listener_vector.erase(
                remove_if(trigger_full_press_listener_vector.begin(),
                          trigger_full_press_listener_vector.end(),
                          [&](const Trigger_listener &elem) { return &elem == &listener; }),
                trigger_full_press_listener_vector.end());
    }

    void Trigger::clear_full_press_listeners() {
        trigger_full_press_listener_vector.clear();
    }

    void Trigger::add_press_listener(const Trigger::Trigger_listener &listener) {
        trigger_press_listener_vector.push_back(listener);
    }

    void Trigger::remove_press_listener(Trigger::Trigger_listener listener) {
        trigger_press_listener_vector.erase(
                remove_if(trigger_press_listener_vector.begin(),
                          trigger_press_listener_vector.end(),
                          [&](const Trigger_listener &elem) { return &elem == &listener; }),
                trigger_press_listener_vector.end());
    }

    void Trigger::clear_press_listeners() {
        trigger_press_listener_vector.clear();
    }

    void Trigger::add_trigger_release_listener(const Trigger::Trigger_listener &listener) {
        trigger_release_listener_vector.push_back(listener);
    }

    void Trigger::remove_trigger_release_listener(Trigger::Trigger_listener listener) {
        trigger_release_listener_vector.erase(
                remove_if(trigger_release_listener_vector.begin(),
                          trigger_release_listener_vector.end(),
                          [&](const Trigger_listener &elem) { return &elem == &listener; }),
                trigger_release_listener_vector.end());
    }

    void Trigger::clear_trigger_release_listeners() {
        trigger_release_listener_vector.clear();
    }

    void Trigger::update(float new_value) {
        if (new_value != value) {
            if (new_value == 1) {
                on_full_press();
            } else if (value == 1) {
                on_release();
            } else {
                on_press(new_value);
            }
            value = new_value;
        }
    }

    void Trigger::on_full_press() {
        for (const auto &listener: trigger_full_press_listener_vector) {
            listener(1);
        }
    }

    void Trigger::on_press(float value) {
        for (const auto &listener: trigger_press_listener_vector) {
            listener(value);
        }
    }

    void Trigger::on_release() {
        for (const auto &listener: trigger_release_listener_vector) {
            listener(0);
        }
    }
} // sdk