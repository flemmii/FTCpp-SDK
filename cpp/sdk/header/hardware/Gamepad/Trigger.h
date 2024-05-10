//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_TRIGGER_H
#define FTCROBOTCONTROLLER_TRIGGER_H

#include <vector>

namespace sdk {
    class Trigger {

    public:
        using Trigger_listener = std::function<void(float)>;

        void add_full_press_listener(const Trigger_listener &listener);

        void remove_full_press_listener(Trigger_listener listener);

        void clear_full_press_listeners();

        void add_press_listener(const Trigger_listener &listener);

        void remove_press_listener(Trigger_listener listener);

        void clear_press_listeners();

        void add_trigger_release_listener(const Trigger_listener &listener);

        void remove_trigger_release_listener(Trigger_listener listener);

        void clear_trigger_release_listeners();

        void update(float new_value);

    private:
        void on_full_press();

        void on_press(float value);

        void on_release();

        std::vector<Trigger_listener> trigger_full_press_listener_vector;
        std::vector<Trigger_listener> trigger_press_listener_vector;
        std::vector<Trigger_listener> trigger_release_listener_vector;
        double value = 0.0;
    };
} // sdk

#endif //FTCROBOTCONTROLLER_TRIGGER_H
