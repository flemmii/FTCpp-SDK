//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Color_sensor.hpp"

namespace sdk
{
    jclass Color_sensor::jclazz;

    Color_sensor::Color_sensor(const jobject &colorSensor) : colorSensor(colorSensor) {}

    Color_sensor::~Color_sensor()
    {
        if (colorSensor)
        {
            attach_thread
                env->DeleteGlobalRef(colorSensor);
            colorSensor = nullptr;
        }
    }

    Color_sensor &Color_sensor::operator=(const Color_sensor &color_sensor)
    {
        if (this != &color_sensor && color_sensor.colorSensor)
        {
            attach_thread this->colorSensor = env->NewGlobalRef(color_sensor.colorSensor);
        }
        return *this;
    }

    Color_sensor &Color_sensor::operator=(const jobject &colorSensor)
    {
        if (this->colorSensor)
        {
            attach_thread
                env->DeleteGlobalRef(this->colorSensor);
        }
        this->colorSensor = colorSensor;
        return *this;
    }

    int Color_sensor::red() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(colorSensor,
                                                                 env->GetMethodID(jclazz, "red", "()I")));
    }

    int Color_sensor::green() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(colorSensor,
                                                                 env->GetMethodID(jclazz, "green", "()I")));
    }

    int Color_sensor::blue() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(colorSensor,
                                                                 env->GetMethodID(jclazz, "blue", "()I")));
    }

    int Color_sensor::alpha() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(colorSensor,
                                                                 env->GetMethodID(jclazz, "alpha", "()I")));
    }

    std::tuple<int, int, int, int> Color_sensor::argb() const
    {
        attach_thread int argb = static_cast<int>(env->CallIntMethod(colorSensor,
                                                                     env->GetMethodID(jclazz, "argb", "()I")));
        return std::make_tuple((argb >> 24) & 0xFF, (argb >> 16) & 0xFF, (argb >> 8) & 0xFF,
                               argb & 0xFF);
    }

    void Color_sensor::enable_led(const bool &enable) const
    {
        attach_thread
            env->CallVoidMethod(colorSensor, env->GetMethodID(jclazz, "enableLed", "(Z)V"),
                                enable);
    }
} // sdk