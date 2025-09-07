//
// Created by fnlg on 03.02.2024.
//

#include "Gamepad/Button.hpp"

void Button::add_press_listener(const Button_listener &listener)
{
    press_listener_vector.push_back(listener);
}

void Button::remove_press_listener(const Button_listener &listener)
{
    press_listener_vector.erase(
        remove_if(press_listener_vector.begin(),
                  press_listener_vector.end(),
                  [&](const Button_listener &elem)
                  { return &elem == &listener; }),
        press_listener_vector.end());
}

void Button::clear_press_listeners()
{
    press_listener_vector.clear();
}

void Button::add_release_listener(const Button_listener &listener)
{
    release_listener_vector.push_back(listener);
}

void Button::remove_release_listener(const Button_listener &listener)
{
    release_listener_vector.erase(
        remove_if(release_listener_vector.begin(),
                  release_listener_vector.end(),
                  [&](const Button_listener &elem)
                  { return &elem == &listener; }),
        release_listener_vector.end());
}

void Button::clear_release_listeners()
{
    release_listener_vector.clear();
}

void Button::update(const bool &new_state)
{
    if (new_state != state)
    {
        if (new_state)
        {
            on_press();
        }
        else
        {
            on_release();
        }
        state = new_state;
    }
}

void Button::on_press()
{
    for (const auto &listener : press_listener_vector)
    {
        listener();
    }
}

void Button::on_release()
{
    for (const auto &listener : release_listener_vector)
    {
        listener();
    }
}