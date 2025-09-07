//
// Created by fnlg on 03.02.2024.
//

#include "Gamepad/Sticks.hpp"

void Sticks::add_update_listener(const Update_listener &listener, bool monitor_left_x,
                                 bool monitor_left_y,
                                 bool monitor_right_x, bool monitor_right_y)
{
    update_listener_vector.push_back(
        {listener, monitor_left_x, monitor_left_y, monitor_right_x, monitor_right_y});
}

void Sticks::remove_update_listener(const Update_listener &listener)
{
    update_listener_vector.erase(
        std::remove_if(update_listener_vector.begin(), update_listener_vector.end(),
                       [&](const Listener_info &elem)
                       { return &elem.listener == &listener; }),
        update_listener_vector.end());
}

void Sticks::clear_update_listener_list()
{
    update_listener_vector.clear();
}

void Sticks::update(const float &left_x, const float &left_y, const float &right_x,
                    const float &right_y)
{
    if (left_x == this->left_x && left_y == this->left_y && right_x == this->right_x &&
        right_y == this->right_y)
        return;

    for (const auto &info : update_listener_vector)
    {
        if ((info.monitor_left_x && left_x != this->left_x) ||
            (info.monitor_left_y && left_y != this->left_y) ||
            (info.monitor_right_x && right_x != this->right_x) ||
            (info.monitor_right_y && right_y != this->right_y))
            info.listener(left_x, left_y, right_x, right_y);
    }

    this->left_x = left_x;
    this->left_y = left_y;
    this->right_x = right_x;
    this->right_y = right_y;
}