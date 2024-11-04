//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Touch_sensor.h"

namespace sdk {
    jclass Touch_sensor::jclazz;

    Touch_sensor::Touch_sensor(const jobject &touchSensor) : touchSensor(touchSensor) {}

    Touch_sensor::~Touch_sensor() {
        if (touchSensor) {
            attach_thread
            env->DeleteGlobalRef(touchSensor);
            touchSensor = nullptr;
        }
    }

    Touch_sensor &Touch_sensor::operator=(const Touch_sensor &touch_sensor) {
        if (this != &touch_sensor && touch_sensor.touchSensor) {
            attach_thread
            this->touchSensor = env->NewGlobalRef(touch_sensor.touchSensor);
        }
        return *this;
    }

    Touch_sensor &Touch_sensor::operator=(const jobject &touchSensor) {
        if (this->touchSensor) {
            attach_thread
            env->DeleteGlobalRef(this->touchSensor);
        }
        this->touchSensor = touchSensor;
        return *this;
    }

    double Touch_sensor::get_value() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(touchSensor,
                                                         env->GetMethodID(jclazz, "getValue",
                                                                          "()D")));
    }

    bool Touch_sensor::is_pressed() const {
        attach_thread
        return env->CallBooleanMethod(touchSensor,
                                      env->GetMethodID(jclazz, "isPressed", "()Z"));
    }
} // sdk