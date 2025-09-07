#include "Gamepad/Better_gamepad.hpp"

using namespace sdk;

Better_gamepad::Better_gamepad(const jobject &gamepad) : Gamepad(gamepad) {}

Better_gamepad &Better_gamepad::operator=(Gamepad &gamepad)
{
    Gamepad::operator=(gamepad);

    sticks.clear_update_listener_list();

    left_stick_button.clear_press_listeners();
    left_stick_button.clear_release_listeners();

    right_stick_button.clear_press_listeners();
    right_stick_button.clear_release_listeners();

    left_trigger.clear_press_listeners();
    left_trigger.clear_update_listeners();
    left_trigger.clear_release_listeners();

    right_trigger.clear_press_listeners();
    right_trigger.clear_update_listeners();
    right_trigger.clear_release_listeners();

    left_bumper.clear_press_listeners();
    left_bumper.clear_release_listeners();

    right_bumper.clear_press_listeners();
    right_bumper.clear_release_listeners();

    a.clear_press_listeners();
    a.clear_release_listeners();

    b.clear_press_listeners();
    b.clear_release_listeners();

    x.clear_press_listeners();
    x.clear_release_listeners();

    y.clear_press_listeners();
    y.clear_release_listeners();

    dpad_up.clear_press_listeners();
    dpad_up.clear_release_listeners();

    dpad_right.clear_press_listeners();
    dpad_right.clear_release_listeners();

    dpad_down.clear_press_listeners();
    dpad_down.clear_release_listeners();

    dpad_left.clear_press_listeners();
    dpad_left.clear_release_listeners();

    guide.clear_press_listeners();
    guide.clear_release_listeners();

    start.clear_press_listeners();
    start.clear_release_listeners();

    back.clear_press_listeners();
    back.clear_release_listeners();

    return *this;
}

Better_gamepad::~Better_gamepad()
{
    sticks.clear_update_listener_list();

    left_stick_button.clear_press_listeners();
    left_stick_button.clear_release_listeners();

    right_stick_button.clear_press_listeners();
    right_stick_button.clear_release_listeners();

    left_trigger.clear_press_listeners();
    left_trigger.clear_update_listeners();
    left_trigger.clear_release_listeners();

    right_trigger.clear_press_listeners();
    right_trigger.clear_update_listeners();
    right_trigger.clear_release_listeners();

    left_bumper.clear_press_listeners();
    left_bumper.clear_release_listeners();

    right_bumper.clear_press_listeners();
    right_bumper.clear_release_listeners();

    a.clear_press_listeners();
    a.clear_release_listeners();

    b.clear_press_listeners();
    b.clear_release_listeners();

    x.clear_press_listeners();
    x.clear_release_listeners();

    y.clear_press_listeners();
    y.clear_release_listeners();

    dpad_up.clear_press_listeners();
    dpad_up.clear_release_listeners();

    dpad_right.clear_press_listeners();
    dpad_right.clear_release_listeners();

    dpad_down.clear_press_listeners();
    dpad_down.clear_release_listeners();

    dpad_left.clear_press_listeners();
    dpad_left.clear_release_listeners();

    guide.clear_press_listeners();
    guide.clear_release_listeners();

    start.clear_press_listeners();
    start.clear_release_listeners();

    back.clear_press_listeners();
    back.clear_release_listeners();
}

void Better_gamepad::update()
{
    Gamepad::update();

    sticks.update(left_stick_x, -left_stick_y, right_stick_x, -right_stick_y);

    left_stick_button.update(sdk::Gamepad::left_stick_button);
    right_stick_button.update(sdk::Gamepad::right_stick_button);

    left_trigger.update(sdk::Gamepad::left_trigger);
    right_trigger.update(sdk::Gamepad::right_trigger);

    left_bumper.update(sdk::Gamepad::left_bumper);
    right_bumper.update(sdk::Gamepad::right_bumper);

    a.update(sdk::Gamepad::a);
    b.update(sdk::Gamepad::b);
    x.update(sdk::Gamepad::x);
    y.update(sdk::Gamepad::y);

    dpad_up.update(sdk::Gamepad::dpad_up);
    dpad_right.update(sdk::Gamepad::dpad_right);
    dpad_down.update(sdk::Gamepad::dpad_down);
    dpad_left.update(sdk::Gamepad::dpad_left);

    guide.update(sdk::Gamepad::guide);
    start.update(sdk::Gamepad::start);
    back.update(sdk::Gamepad::back);
}

namespace gamepads
{
    Better_gamepad gamepad1;
    Better_gamepad gamepad2;

    void init()
    {
        gamepad1 = sdk::gamepads::gamepad1;
        gamepad2 = sdk::gamepads::gamepad2;
    }

    void update()
    {
        gamepad1.update();
        gamepad2.update();
    }
}