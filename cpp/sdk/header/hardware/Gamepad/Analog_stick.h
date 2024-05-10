//
// Created by fnlg on 03.02.2024.
//

#ifndef FTCROBOTCONTROLLER_ANALOG_STICK_H
#define FTCROBOTCONTROLLER_ANALOG_STICK_H

#include <vector>

using namespace std;

namespace sdk {
    class Analog_stick {
    public:
        using Update_listener = function<void(float, float)>;

        void add_update_listener(const Update_listener &listener);

        void remove_update_listener(Update_listener listener);

        void clear_update_listener_list();

        void update(float x, float y);

    private:
        std::vector<Update_listener> update_listener_vector;
    };
}


#endif //FTCROBOTCONTROLLER_ANALOG_STICK_H
