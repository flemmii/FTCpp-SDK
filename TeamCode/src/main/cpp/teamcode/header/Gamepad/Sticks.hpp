//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_ANALOG_STICKS_H
#define FTCROBOTCONTROLLER_ANALOG_STICKS_H

#include <vector>
#include <algorithm>
#include <functional>

class Sticks
{
public:
    using Update_listener = std::function<void(float, float, float, float)>;

    struct Listener_info
    {
        Update_listener listener;
        bool monitor_left_x;
        bool monitor_left_y;
        bool monitor_right_x;
        bool monitor_right_y;

        Listener_info &operator=(const Listener_info &other)
        {
            if (this != &other)
            {
                listener = other.listener;
                monitor_left_x = other.monitor_left_x;
                monitor_left_y = other.monitor_left_y;
                monitor_right_x = other.monitor_right_x;
                monitor_right_y = other.monitor_right_y;
            }
            return *this;
        }
    };

    void add_update_listener(const Update_listener &listener, bool monitor_left_x = true,
                             bool monitor_left_y = true, bool monitor_right_x = true,
                             bool monitor_right_y = true);

    void remove_update_listener(const Update_listener &listener);

    void clear_update_listener_list();

    void
    update(const float &left_x, const float &left_y, const float &right_x, const float &right_y);

private:
    std::vector<Listener_info> update_listener_vector;

    float left_x = 0, left_y = 0, right_x = 0, right_y = 0;
};

#endif // FTCROBOTCONTROLLER_ANALOG_STICKS_H
