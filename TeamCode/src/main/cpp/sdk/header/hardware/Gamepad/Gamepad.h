//
// Created by fnlg on 30.12.2023.
//

#ifndef FTCROBOTCONTROLLER_GAMEPAD_H
#define FTCROBOTCONTROLLER_GAMEPAD_H

#include "utils.h"
#include "Analog_stick.h"
#include "Analog_sticks.h"
#include "Button.h"
#include "Trigger.h"

namespace sdk {

    extern jclass Gamepad;

    class Gamepad {
    public:
        jobject gamepad{};

        Analog_stick left_stick{};
        Analog_stick right_stick{};

        Analog_sticks both_sticks{};

        Button left_stick_button{};

        Button right_stick_button{};

        Trigger left_trigger{};
        Trigger right_trigger{};

        Button left_bumper{};
        Button right_bumper{};

        Button a{};
        Button b{};
        Button x{};
        Button y{};

        Button dpad_up{};
        Button dpad_right{};
        Button dpad_down{};
        Button dpad_left{};

        Button guide{};
        Button start{};
        Button back{};

        /*
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
        */

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
