//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Analog_input.h"

namespace sdk {
    jclass AnalogInput;

    Analog_input::Analog_input(jobject analogInput) : analogInput(analogInput) {}

    Analog_input::~Analog_input() {
        if (analogInput) {
            attach_thread
            env->DeleteGlobalRef(analogInput);
            analogInput = nullptr;
        }
    }

    Analog_input &Analog_input::operator=(jobject analogInput) {
        if (this->analogInput) {
            attach_thread
            env->DeleteGlobalRef(this->analogInput);
        }
        this->analogInput = analogInput;
        return *this;
    }

    int Analog_input::get_voltage() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(analogInput,
                                                   env->GetMethodID(AnalogInput, "getVoltage",
                                                                    "()I")));
    }

    int Analog_input::get_max_voltage() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(analogInput,
                                                   env->GetMethodID(AnalogInput, "getMaxVoltage",
                                                                    "()I")));
    }

    std::string Analog_input::get_device_name() const {
        attach_thread
        auto jStr = (jstring) (env->CallObjectMethod(analogInput,
                                                     env->GetMethodID(AnalogInput,
                                                                      "getDeviceName",
                                                                      "()Ljava/lang/String;")));

        const char *cStr = env->GetStringUTFChars(jStr, nullptr);
        std::string str(cStr);
        env->DeleteLocalRef(jStr);

        return str;
    }

    std::string Analog_input::get_connection_info() const {
        attach_thread
        auto jStr = (jstring) (env->CallObjectMethod(analogInput,
                                                     env->GetMethodID(AnalogInput,
                                                                      "getConnectionInfo",
                                                                      "()Ljava/lang/String;")));

        const char *cStr = env->GetStringUTFChars(jStr, nullptr);
        std::string str(cStr);
        env->DeleteLocalRef(jStr);

        return str;
    }

    int Analog_input::get_version() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(analogInput,
                                                   env->GetMethodID(AnalogInput, "getVersion",
                                                                    "()I")));
    }

    void Analog_input::reset_device_configuration_for_op_mode() const {
        attach_thread
        env->CallVoidMethod(analogInput,
                            env->GetMethodID(AnalogInput, "resetDeviceConfigurationForOpMode",
                                             "()V"));
    }

    void Analog_input::close() const {
        attach_thread
        env->CallVoidMethod(analogInput, env->GetMethodID(AnalogInput, "close", "()V"));
    }
} // sdk