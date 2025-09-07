//
// Created by fnlg on 30.12.2023.
//

#ifndef FTCROBOTCONTROLLER_BETTER_GAMEPAD_H
#define FTCROBOTCONTROLLER_BETTER_GAMEPAD_H

#include "hardware/Gamepad.hpp"

#include "Sticks.hpp"
#include "Button.hpp"
#include "Trigger.hpp"

class Better_gamepad : public sdk::Gamepad
{
public:
    Better_gamepad() = default;

    Better_gamepad(const jobject &gamepad);

    ~Better_gamepad();

    Better_gamepad &operator=(Gamepad &gamepad);

    Sticks sticks{};

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

    void update();
};

namespace gamepads
{
    extern Better_gamepad gamepad1;
    extern Better_gamepad gamepad2;

    void init();

    void update();
}

#endif // FTCROBOTCONTROLLER_BETTER_GAMEPAD_H
