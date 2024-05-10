//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_BUTTON_H
#define FTCROBOTCONTROLLER_BUTTON_H

#include <vector>

namespace sdk {
    class Button {
    public:
        using Button_listener = std::function<void()>;

        void add_button_press_listener(const Button_listener &listener);

        void remove_button_press_listener(Button_listener listener);

        void clear_button_press_listeners();

        void add_button_release_listener(const Button_listener &listener);

        void remove_button_release_listener(Button_listener listener);

        void clear_button_release_listeners();

        void update(bool new_state);

    private:
        void on_press();

        void on_release();

        std::vector<Button_listener> button_press_listener_vector;
        std::vector<Button_listener> button_release_listener_vector;
        bool state = false;
    };
}


#endif //FTCROBOTCONTROLLER_BUTTON_H
