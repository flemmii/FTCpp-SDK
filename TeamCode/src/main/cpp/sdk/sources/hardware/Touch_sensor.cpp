//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Touch_sensor.h"

namespace sdk {
    jclass TouchSensor;

    Touch_sensor::Touch_sensor(jobject touchSensor) {
        attach_thread
        this->touchSensor = env->NewGlobalRef(touchSensor);
    }

    Touch_sensor::~Touch_sensor() {
        if (touchSensor) {
            attach_thread
            env->DeleteGlobalRef(touchSensor);
        }
    }

    double Touch_sensor::get_value() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(touchSensor,
                                                         env->GetMethodID(TouchSensor, "getValue",
                                                                          "()D")));
    }

    bool Touch_sensor::is_pressed() const {
        attach_thread
        return env->CallBooleanMethod(touchSensor,
                                      env->GetMethodID(TouchSensor, "isPressed", "()Z"));
    }
} // sdk