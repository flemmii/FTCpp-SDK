//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Digital_channel.h"

namespace sdk {
    jclass Digital_channel::jclazz;
    jclass Digital_channel::jclazz_Mode;

    Digital_channel::Digital_channel(const jobject &digitalChannel) : digitalChannel(
            digitalChannel) {}

    Digital_channel::~Digital_channel() {
        if (digitalChannel) {
            attach_thread
            env->DeleteGlobalRef(digitalChannel);
            digitalChannel = nullptr;
        }
    }

    Digital_channel &Digital_channel::operator=(const Digital_channel &digital_channel) {
        if (this != &digital_channel && digital_channel.digitalChannel) {
            attach_thread
            this->digitalChannel = env->NewGlobalRef(digital_channel.digitalChannel);
        }
        return *this;
    }

    Digital_channel &Digital_channel::operator=(const jobject &digitalChannel) {
        if (this->digitalChannel) {
            attach_thread
            env->DeleteGlobalRef(this->digitalChannel);
        }
        this->digitalChannel = digitalChannel;
        return *this;
    }

    Digital_channel::Mode Digital_channel::get_mode() const {
        attach_thread
        jobject mode = env->CallObjectMethod(digitalChannel,
                                             env->GetMethodID(jclazz, "getMode",
                                                              "()Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;"));

        auto name = (jstring) env->CallObjectMethod(mode,
                                                    env->GetMethodID(jclazz_Mode,
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *modeName = env->GetStringUTFChars(name, nullptr);
        std::string strModeName(modeName);
        env->DeleteLocalRef(name);

        env->DeleteLocalRef(mode);

        if (strModeName == "INPUT")
            return Mode::INPUT;
        else if (strModeName == "OUTPUT")
            return Mode::OUTPUT;
        return Mode::INPUT;
    }

    void Digital_channel::set_mode(const Mode &mode) const {
        attach_thread
        jobject modeObj;
        if (mode == Mode::INPUT) {
            modeObj = env->GetStaticObjectField(jclazz_Mode,
                                                env->GetStaticFieldID(jclazz_Mode, "INPUT",
                                                                      "Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;"));
        } else {
            modeObj = env->GetStaticObjectField(jclazz_Mode,
                                                env->GetStaticFieldID(jclazz_Mode, "OUTPUT",
                                                                      "Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;"));
        }
        env->CallVoidMethod(digitalChannel,
                            env->GetMethodID(jclazz, "setMode",
                                             "(Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;)V"),
                            modeObj);
        env->DeleteLocalRef(modeObj);
    }

    bool Digital_channel::get_state() const {
        attach_thread
        return env->CallBooleanMethod(digitalChannel,
                                      env->GetMethodID(jclazz, "getState", "()Z"));
    }

    void Digital_channel::set_state(const bool &state) const {
        attach_thread
        env->CallVoidMethod(digitalChannel,
                            env->GetMethodID(jclazz, "setState", "(Z)V"),
                            state);
    }
} // sdk