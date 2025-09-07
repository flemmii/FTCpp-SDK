//
// Created by fnlg on 03.02.2024.
//

#include "Gamepad/Trigger.hpp"

void Trigger::add_update_listener(const Update_listener &listener)
{
    update_listener_vector.push_back(listener);
}

void Trigger::remove_update_listener(const Update_listener &listener)
{
    update_listener_vector.erase(
        remove_if(update_listener_vector.begin(),
                  update_listener_vector.end(),
                  [&](const Update_listener &elem)
                  { return &elem == &listener; }),
        update_listener_vector.end());
}

void Trigger::clear_update_listeners()
{
    update_listener_vector.clear();
}

void Trigger::add_press_listener(const Trigger_listener &listener)
{
    press_listener_vector.push_back(listener);
}

void Trigger::remove_press_listener(const Trigger_listener &listener)
{
    press_listener_vector.erase(
        remove_if(press_listener_vector.begin(),
                  press_listener_vector.end(),
                  [&](const Trigger_listener &elem)
                  { return &elem == &listener; }),
        press_listener_vector.end());
}

void Trigger::clear_press_listeners()
{
    press_listener_vector.clear();
}

void Trigger::add_release_listener(const Trigger_listener &listener)
{
    release_listener_vector.push_back(listener);
}

void Trigger::remove_release_listener(const Trigger_listener &listener)
{
    release_listener_vector.erase(
        remove_if(release_listener_vector.begin(),
                  release_listener_vector.end(),
                  [&](const Trigger_listener &elem)
                  { return &elem == &listener; }),
        release_listener_vector.end());
}

void Trigger::clear_release_listeners()
{
    release_listener_vector.clear();
}

void Trigger::update(const float &new_value)
{
    if (new_value != value)
    {
        on_update(new_value);

        if (new_value >= 0.5 && value < 0.5)
            on_press();
        else if (new_value < 0.5 && value >= 0.5)
            on_release();

        value = new_value;
    }
}

void Trigger::on_update(const float &value)
{
    for (const auto &listener : update_listener_vector)
    {
        listener(value);
    }
}

void Trigger::on_press()
{
    for (const auto &listener : press_listener_vector)
    {
        listener();
    }
}

void Trigger::on_release()
{
    for (const auto &listener : release_listener_vector)
    {
        listener();
    }
}