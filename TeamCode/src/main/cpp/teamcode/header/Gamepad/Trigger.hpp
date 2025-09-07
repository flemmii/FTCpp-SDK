//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_TRIGGER_H
#define FTCROBOTCONTROLLER_TRIGGER_H

#include <vector>
#include <algorithm>
#include <functional>

class Trigger
{

public:
    using Trigger_listener = std::function<void()>;
    using Update_listener = std::function<void(float)>;

    void add_update_listener(const Update_listener &listener);

    void remove_update_listener(const Update_listener &listener);

    void clear_update_listeners();

    void add_press_listener(const Trigger_listener &listener);

    void remove_press_listener(const Trigger_listener &listener);

    void clear_press_listeners();

    void add_release_listener(const Trigger_listener &listener);

    void remove_release_listener(const Trigger_listener &listener);

    void clear_release_listeners();

    void update(const float &new_value);

private:
    void on_update(const float &value);

    void on_press();

    void on_release();

    std::vector<Update_listener> update_listener_vector;
    std::vector<Trigger_listener> press_listener_vector;
    std::vector<Trigger_listener> release_listener_vector;
    float value = 0;
};

#endif // FTCROBOTCONTROLLER_TRIGGER_H
