//
// Created by fnlg on 03.02.2024.
//

#include "hardware/Gamepad/Button.h"

namespace sdk {

    void Button::add_button_press_listener(const std::function<void()> &listener) {
        button_press_listener_vector.push_back(listener);
    }

    void Button::remove_button_press_listener(Button::Button_listener listener) {
        button_press_listener_vector.erase(
                remove_if(button_press_listener_vector.begin(),
                          button_press_listener_vector.end(),
                          [&](const Button_listener &elem) { return &elem == &listener; }),
                button_press_listener_vector.end());
    }

    void Button::clear_button_press_listeners() {
        button_press_listener_vector.clear();
    }

    void Button::add_button_release_listener(const Button::Button_listener &listener) {
        button_release_listener_vector.push_back(listener);
    }

    void Button::remove_button_release_listener(Button::Button_listener listener) {
        button_release_listener_vector.erase(
                remove_if(button_release_listener_vector.begin(),
                          button_release_listener_vector.end(),
                          [&](const Button_listener &elem) { return &elem == &listener; }),
                button_release_listener_vector.end());
    }

    void Button::clear_button_release_listeners() {
        button_release_listener_vector.clear();
    }

    void Button::update(bool new_state) {
        if (new_state != state) {
            if (new_state) {
                on_press();
            } else {
                on_release();
            }
            state = new_state;
        }
    }

    void Button::on_press() {
        for (const auto &listener: button_press_listener_vector) {
            listener();
        }
    }

    void Button::on_release() {
        for (const auto &listener: button_release_listener_vector) {
            listener();
        }
    }
}