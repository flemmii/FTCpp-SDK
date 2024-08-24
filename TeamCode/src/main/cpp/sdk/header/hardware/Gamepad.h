//
// Created by fnlg on 30.12.2023.
//

#ifndef FTCROBOTCONTROLLER_GAMEPAD_H
#define FTCROBOTCONTROLLER_GAMEPAD_H

#include "utils.h"

namespace sdk {
    class Gamepad {
    public:
        static jclass jclazz;
        jobject gamepad{};

        float left_stick_x = 0;
        float left_stick_y = 0;
        float right_stick_x = 0;
        float right_stick_y = 0;

        bool left_stick_button = false;
        bool right_stick_button = false;

        float left_trigger = 0;
        float right_trigger = 0;

        bool left_bumper = false;
        bool right_bumper = false;

        bool a = false;
        bool b = false;
        bool x = false;
        bool y = false;

        bool dpad_up = false;
        bool dpad_right = false;
        bool dpad_down = false;
        bool dpad_left = false;

        bool guide = false;
        bool start = false;
        bool back = false;

        bool circle = false;
        bool cross = false;
        bool triangle = false;
        bool square = false;

        bool share = false;
        bool options = false;

        bool touchpad = false;
        bool touchpad_finger_1 = false;
        bool touchpad_finger_2 = false;
        float touchpad_finger_1_x = 0;
        float touchpad_finger_1_y = 0;
        float touchpad_finger_2_x = 0;
        float touchpad_finger_2_y = 0;

        bool ps = false;

        Gamepad() = default;

        Gamepad(jobject gamepad);

        ~Gamepad();

        Gamepad &operator=(jobject gamepad);

        void update();

        void rumble(int duration_ms) const;

        void rumble(double rumble1, double rumble2, int duration_ms) const;

        void stop_rumble() const;

        [[nodiscard]] bool is_rumbling() const;
    };

    namespace gamepads {
        void update();

        extern class Gamepad gamepad1;

        extern class Gamepad gamepad2;

    }
} // sdk

#endif //FTCROBOTCONTROLLER_GAMEPAD_H
