//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_ANALOG_STICKS_H
#define FTCROBOTCONTROLLER_ANALOG_STICKS_H

#include <vector>

namespace sdk {
    class Analog_sticks {

    public:
        using Update_listener = std::function<void(float, float, float, float)>;

        void add_update_listener(const Update_listener &listener);

        void remove_update_listener(Update_listener listener);

        void clear_update_listener_list();

        void update(float left_x, float left_y, float right_x, float right_y);

    private:
        std::vector<Update_listener> update_listener_vector;
    };
} // sdk

#endif //FTCROBOTCONTROLLER_ANALOG_STICKS_H
