#include "hardware/Gamepad/Gamepad.h"

namespace sdk {
    jclass Gamepad;

    Gamepad::Gamepad(jobject gamepad) {
        attach_thread
        this->gamepad = env->NewGlobalRef(gamepad);
    }

    void Gamepad::update() {
        attach_thread

        auto left_stick_x = static_cast<float> (env->GetFloatField(gamepad,
                                                                   env->GetFieldID(sdk::Gamepad,
                                                                                   "left_stick_x",
                                                                                   "F")));
        auto left_stick_y = static_cast<float> (env->GetFloatField(gamepad,
                                                                   env->GetFieldID(sdk::Gamepad,
                                                                                   "left_stick_y",
                                                                                   "F")));
        auto right_stick_x = static_cast<float> (env->GetFloatField(gamepad,
                                                                    env->GetFieldID(sdk::Gamepad,
                                                                                    "right_stick_x",
                                                                                    "F")));
        auto right_stick_y = static_cast<float> (-env->GetFloatField(gamepad,
                                                                     env->GetFieldID(sdk::Gamepad,
                                                                                     "right_stick_y",
                                                                                     "F")));

        left_stick.update(left_stick_x, -left_stick_y);
        right_stick.update(right_stick_x, -right_stick_y);

        both_sticks.update(left_stick_x, -left_stick_y, right_stick_x, -right_stick_y);

        left_stick_button.update(env->GetBooleanField(gamepad,
                                                      env->GetFieldID(sdk::Gamepad,
                                                                      "left_stick_button",
                                                                      "Z")));
        right_stick_button.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad,
                                                                                "right_stick_button",
                                                                                "Z")));

        left_trigger.update(
                static_cast<float> (env->GetFloatField(gamepad, env->GetFieldID(sdk::Gamepad,
                                                                                "left_trigger",
                                                                                "F"))));
        right_trigger.update(static_cast<float> (env->GetFloatField(gamepad,
                                                                    env->GetFieldID(sdk::Gamepad,
                                                                                    "right_trigger",
                                                                                    "F"))));

        left_bumper.update(env->GetBooleanField(gamepad,
                                                env->GetFieldID(sdk::Gamepad, "left_bumper", "Z")));
        right_bumper.update(env->GetBooleanField(gamepad,
                                                 env->GetFieldID(sdk::Gamepad, "right_bumper",
                                                                 "Z")));

        a.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "a", "Z")));
        b.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "b", "Z")));
        x.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "x", "Z")));
        y.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "y", "Z")));

        dpad_up.update(
                env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "dpad_up", "Z")));
        dpad_right.update(
                env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "dpad_right", "Z")));
        dpad_down.update(
                env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "dpad_down", "Z")));
        dpad_left.update(
                env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "dpad_left", "Z")));

        guide.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "guide", "Z")));
        start.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "start", "Z")));
        back.update(env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "back", "Z")));

        /*
        circle = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "circle", "Z"));
        cross = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "cross", "Z"));
        triangle = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "triangle", "Z"));
        square = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "square", "Z"));

        share = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "share", "Z"));
        options = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "options", "Z"));

        touchpad = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad", "Z"));
        touchpad_finger_1 = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad_finger_1", "Z"));
        touchpad_finger_2 = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad_finger_2", "Z"));
        touchpad_finger_1_x = static_cast<float> (env->GetFloatField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad_finger_1_x", "F")));
        touchpad_finger_1_y = static_cast<float> (env->GetFloatField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad_finger_1_y", "F")));
        touchpad_finger_2_x = static_cast<float> (env->GetFloatField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad_finger_2_x", "F")));
        touchpad_finger_2_y = static_cast<float> (env->GetFloatField(gamepad, env->GetFieldID(sdk::Gamepad, "touchpad_finger_2_y", "F")));

        ps = env->GetBooleanField(gamepad, env->GetFieldID(sdk::Gamepad, "ps", "Z"));
        */
    }

    void Gamepad::rumble(int duration_ms) const {
        attach_thread
        env->CallVoidMethod(gamepad, env->GetMethodID(sdk::Gamepad, "rumble", "(I)V"),
                            static_cast<jint>(duration_ms));
    }

    void Gamepad::rumble(double rumble1, double rumble2, int duration_ms) const {
        attach_thread
        env->CallVoidMethod(gamepad, env->GetMethodID(sdk::Gamepad, "rumble", "(DDI)V"),
                            static_cast<jdouble>(rumble1), static_cast<jdouble>(rumble2),
                            static_cast<jint>(duration_ms));
    }

    void Gamepad::stop_rumble() const {
        attach_thread
        env->CallVoidMethod(gamepad, env->GetMethodID(sdk::Gamepad, "stopRumble", "()V"));
    }

    bool Gamepad::is_rumbling() const {
        attach_thread
        return env->CallBooleanMethod(gamepad, env->GetMethodID(sdk::Gamepad, "isRumbling", "()Z"));
    }

    namespace gamepads {
        class Gamepad gamepad1;

        class Gamepad gamepad2;

        void update() {
            gamepad1.update();
            gamepad2.update();
        }
    }
} // sdk