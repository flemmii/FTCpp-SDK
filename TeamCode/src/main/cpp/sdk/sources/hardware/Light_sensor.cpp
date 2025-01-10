//
// Created by flemmi on 04.11.24.
//

#include "hardware/Light_sensor.h"

namespace sdk {
    jclass Light_sensor::jclazz = nullptr;

    Light_sensor::Light_sensor(const jobject &lightSensor) : lightSensor(lightSensor) {}

    Light_sensor::~Light_sensor() {
        if (lightSensor) {
            attach_thread
            env->DeleteGlobalRef(lightSensor);
            lightSensor = nullptr;
        }
    }

    Light_sensor &Light_sensor::operator=(const Light_sensor &light_sensor) {
        if (this != &light_sensor && light_sensor.lightSensor) {
            attach_thread
            this->lightSensor = env->NewGlobalRef(light_sensor.lightSensor);
        }
        return *this;
    }

    Light_sensor &Light_sensor::operator=(const jobject &lightSensor) {
        if (this->lightSensor) {
            attach_thread
            env->DeleteGlobalRef(this->lightSensor);
        }
        this->lightSensor = lightSensor;
        return *this;
    }

    double Light_sensor::get_light_detected() const {
        attach_thread
        return env->CallDoubleMethod(lightSensor,
                                     env->GetMethodID(jclazz, "getLightDetected", "()D"));
    }

    double Light_sensor::get_raw_light_detected() const {
        attach_thread
        return env->CallDoubleMethod(lightSensor,
                                     env->GetMethodID(jclazz, "getRawLightDetected", "()D"));
    }

    double Light_sensor::get_raw_light_detected_max() const {
        attach_thread
        return env->CallDoubleMethod(lightSensor,
                                     env->GetMethodID(jclazz, "getRawLightDetectedMax", "()D"));
    }

    void Light_sensor::enable_led(const bool &enable) const {
        attach_thread
        env->CallVoidMethod(lightSensor, env->GetMethodID(jclazz, "enableLed", "(Z)V"), enable);
    }

    std::string Light_sensor::status() const {
        attach_thread
        auto jstr = (jstring) env->CallObjectMethod(lightSensor, env->GetMethodID(jclazz, "status",
                                                                                  "()Ljava/lang/String;"));

        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        std::string str(cstr);
        env->DeleteLocalRef(jstr);

        return str;
    }
} // sdk