#include "hardware/Gamepad.h"

namespace sdk {
    jclass Gamepad::jclazz;

    Gamepad::Gamepad(jobject gamepad) : gamepad(gamepad) {}

    Gamepad::~Gamepad() {
        if (gamepad) {
            attach_thread
            env->DeleteGlobalRef(gamepad);
            gamepad = nullptr;
        }
    }

    class Gamepad &Gamepad::operator=(jobject gamepad) {
        if (this->gamepad) {
            attach_thread
            env->DeleteGlobalRef(this->gamepad);
        }
        this->gamepad = gamepad;
        return *this;
    }

    void Gamepad::update() {
        attach_thread

        left_stick_x = static_cast<float> (env->GetFloatField(gamepad,
                                                              env->GetFieldID(jclazz,
                                                                              "left_stick_x",
                                                                              "F")));
        left_stick_y = static_cast<float> (env->GetFloatField(gamepad,
                                                              env->GetFieldID(jclazz,
                                                                              "left_stick_y",
                                                                              "F")));
        right_stick_x = static_cast<float> (env->GetFloatField(gamepad,
                                                               env->GetFieldID(jclazz,
                                                                               "right_stick_x",
                                                                               "F")));
        right_stick_y = static_cast<float> (env->GetFloatField(gamepad,
                                                               env->GetFieldID(jclazz,
                                                                               "right_stick_y",
                                                                               "F")));

        left_stick_button = env->GetBooleanField(gamepad,
                                                 env->GetFieldID(jclazz,
                                                                 "left_stick_button",
                                                                 "Z"));
        right_stick_button = env->GetBooleanField(gamepad, env->GetFieldID(jclazz,
                                                                           "right_stick_button",
                                                                           "Z"));

        left_trigger =
                static_cast<float> (env->GetFloatField(gamepad, env->GetFieldID(jclazz,
                                                                                "left_trigger",
                                                                                "F")));
        right_trigger = static_cast<float> (env->GetFloatField(gamepad,
                                                               env->GetFieldID(jclazz,
                                                                               "right_trigger",
                                                                               "F")));

        left_bumper = env->GetBooleanField(gamepad,
                                           env->GetFieldID(jclazz, "left_bumper", "Z"));
        right_bumper = env->GetBooleanField(gamepad,
                                            env->GetFieldID(jclazz, "right_bumper",
                                                            "Z"));

        a = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "a", "Z"));
        b = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "b", "Z"));
        x = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "x", "Z"));
        y = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "y", "Z"));

        dpad_up =
                env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "dpad_up", "Z"));
        dpad_right =
                env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "dpad_right", "Z"));
        dpad_down =
                env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "dpad_down", "Z"));
        dpad_left =
                env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "dpad_left", "Z"));

        guide = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "guide", "Z"));
        start = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "start", "Z"));
        back = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "back", "Z"));

        circle = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "circle", "Z"));
        cross = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "cross", "Z"));
        triangle = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "triangle", "Z"));
        square = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "square", "Z"));

        share = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "share", "Z"));
        options = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "options", "Z"));

        touchpad = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "touchpad", "Z"));
        touchpad_finger_1 = env->GetBooleanField(gamepad,
                                                 env->GetFieldID(jclazz, "touchpad_finger_1",
                                                                 "Z"));
        touchpad_finger_2 = env->GetBooleanField(gamepad,
                                                 env->GetFieldID(jclazz, "touchpad_finger_2",
                                                                 "Z"));
        touchpad_finger_1_x = static_cast<float> (env->GetFloatField(gamepad,
                                                                     env->GetFieldID(jclazz,
                                                                                     "touchpad_finger_1_x",
                                                                                     "F")));
        touchpad_finger_1_y = static_cast<float> (env->GetFloatField(gamepad,
                                                                     env->GetFieldID(jclazz,
                                                                                     "touchpad_finger_1_y",
                                                                                     "F")));
        touchpad_finger_2_x = static_cast<float> (env->GetFloatField(gamepad,
                                                                     env->GetFieldID(jclazz,
                                                                                     "touchpad_finger_2_x",
                                                                                     "F")));
        touchpad_finger_2_y = static_cast<float> (env->GetFloatField(gamepad,
                                                                     env->GetFieldID(jclazz,
                                                                                     "touchpad_finger_2_y",
                                                                                     "F")));

        ps = env->GetBooleanField(gamepad, env->GetFieldID(jclazz, "ps", "Z"));
    }

    void Gamepad::rumble(int duration_ms) const {
        attach_thread
        env->CallVoidMethod(gamepad, env->GetMethodID(jclazz, "rumble", "(I)V"),
                            static_cast<jint>(duration_ms));
    }

    void Gamepad::rumble(double rumble1, double rumble2, int duration_ms) const {
        attach_thread
        env->CallVoidMethod(gamepad, env->GetMethodID(jclazz, "rumble", "(DDI)V"),
                            static_cast<jdouble>(rumble1), static_cast<jdouble>(rumble2),
                            static_cast<jint>(duration_ms));
    }

    void Gamepad::stop_rumble() const {
        attach_thread
        env->CallVoidMethod(gamepad, env->GetMethodID(jclazz, "stopRumble", "()V"));
    }

    bool Gamepad::is_rumbling() const {
        attach_thread
        return env->CallBooleanMethod(gamepad, env->GetMethodID(jclazz, "isRumbling", "()Z"));
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