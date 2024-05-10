//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Digital_channel.h"

namespace sdk {
    jclass DigitalChannel;

    Digital_channel::Digital_channel(jobject digitalChannel) {
        attach_thread
        this->digitalChannel = env->NewGlobalRef(digitalChannel);
    }

    Digital_channel::~Digital_channel() {
        if (digitalChannel) {
            attach_thread
            env->DeleteGlobalRef(digitalChannel);
        }
    }

    Digital_channel::Mode Digital_channel::get_mode() const {
        attach_thread
        jobject mode = env->CallObjectMethod(digitalChannel,
                                             env->GetMethodID(DigitalChannel, "getMode",
                                                              "()Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;"));

        auto name = (jstring) env->CallObjectMethod(mode,
                                                    env->GetMethodID(env->GetObjectClass(mode),
                                                                     "name",
                                                                     "()Ljava/lang/String;"));

        const char *modeName = env->GetStringUTFChars(name, nullptr);
        std::string strModeName(modeName);
        env->ReleaseStringUTFChars(name, modeName);

        env->DeleteLocalRef(mode);

        if (strModeName == "INPUT")
            return Mode::INPUT;
        else if (strModeName == "OUTPUT")
            return Mode::OUTPUT;
        return Mode::INPUT;
    }

    void Digital_channel::set_mode(Mode mode) const {
        attach_thread
        jobject modeObj;
        if (mode == Mode::INPUT) {
            modeObj = env->GetStaticObjectField(DigitalChannel,
                                                env->GetStaticFieldID(DigitalChannel, "Mode.INPUT",
                                                                      "Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;"));
        } else {
            modeObj = env->GetStaticObjectField(DigitalChannel,
                                                env->GetStaticFieldID(DigitalChannel, "Mode.OUTPUT",
                                                                      "Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;"));
        }
        env->CallVoidMethod(digitalChannel,
                            env->GetMethodID(DigitalChannel, "setMode",
                                             "(Lcom/qualcomm/robotcore/hardware/DigitalChannel$Mode;)V"),
                            modeObj);
        env->DeleteLocalRef(modeObj);
    }

    bool Digital_channel::get_state() const {
        attach_thread
        return env->CallBooleanMethod(digitalChannel,
                                      env->GetMethodID(DigitalChannel, "getState", "()Z"));
    }

    void Digital_channel::set_state(bool state) const {
        attach_thread
        env->CallVoidMethod(digitalChannel,
                            env->GetMethodID(DigitalChannel, "setState", "(Z)V"),
                            state);
    }
} // sdk