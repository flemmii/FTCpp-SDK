//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_BUTTON_H
#define FTCROBOTCONTROLLER_BUTTON_H

#include <vector>
#include <algorithm>
#include <functional>

class Button
{
public:
    using Button_listener = std::function<void()>;

    void add_press_listener(const Button_listener &listener);

    void remove_press_listener(const Button_listener &listener);

    void clear_press_listeners();

    void add_release_listener(const Button_listener &listener);

    void remove_release_listener(const Button_listener &listener);

    void clear_release_listeners();

    void update(const bool &new_state);

private:
    void on_press();

    void on_release();

    std::vector<Button_listener> press_listener_vector;
    std::vector<Button_listener> release_listener_vector;
    bool state = false;
};

#endif // FTCROBOTCONTROLLER_BUTTON_H
