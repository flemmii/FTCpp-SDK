//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Color_sensor.h"

namespace sdk {
    jclass ColorSensor;

    Color_sensor::Color_sensor(jobject colorSensor) {
        attach_thread
        this->colorSensor = env->NewGlobalRef(colorSensor);
    }

    int Color_sensor::red() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(colorSensor,
                                                   env->GetMethodID(ColorSensor, "red", "()I")));
    }

    int Color_sensor::green() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(colorSensor,
                                                   env->GetMethodID(ColorSensor, "green", "()I")));
    }

    int Color_sensor::blue() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(colorSensor,
                                                   env->GetMethodID(ColorSensor, "blue", "()I")));
    }

    int Color_sensor::alpha() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(colorSensor,
                                                   env->GetMethodID(ColorSensor, "alpha", "()I")));
    }

    int Color_sensor::argb() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(colorSensor,
                                                   env->GetMethodID(ColorSensor, "argb", "()I")));
    }

    void Color_sensor::enable_led(bool enable) const {
        attach_thread
        env->CallVoidMethod(colorSensor, env->GetMethodID(ColorSensor, "enableLed", "(Z)V"),
                            enable);
    }
} // sdk