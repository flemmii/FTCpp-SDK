//
// Created by flemmi on 05.11.24.
//

#include "hardware/Modern_robotics_color_beacon.h"

namespace sdk {
    jclass Modern_robotics_color_beacon::jclazz;

    Modern_robotics_color_beacon::Modern_robotics_color_beacon(
            const jobject &modernRoboticsColorBeacon) : modernRoboticsColorBeacon(
            modernRoboticsColorBeacon) {}

    Modern_robotics_color_beacon::~Modern_robotics_color_beacon() {
        if (modernRoboticsColorBeacon) {
            attach_thread
            env->DeleteGlobalRef(modernRoboticsColorBeacon);
            modernRoboticsColorBeacon = nullptr;
        }
    }

    Modern_robotics_color_beacon &Modern_robotics_color_beacon::operator=(
            const Modern_robotics_color_beacon &modern_robotics_color_beacon) {
        if (this != &modern_robotics_color_beacon &&
            modern_robotics_color_beacon.modernRoboticsColorBeacon) {
            attach_thread
            this->modernRoboticsColorBeacon = env->NewGlobalRef(
                    modern_robotics_color_beacon.modernRoboticsColorBeacon);
        }
        return *this;
    }

    Modern_robotics_color_beacon &
    Modern_robotics_color_beacon::operator=(const jobject &modernRoboticsColorBeacon) {
        if (this->modernRoboticsColorBeacon) {
            attach_thread
            env->DeleteGlobalRef(this->modernRoboticsColorBeacon);
        }
        this->modernRoboticsColorBeacon = modernRoboticsColorBeacon;
        return *this;
    }

    void Modern_robotics_color_beacon::turn_LEDs_off() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon,
                            env->GetMethodID(jclazz, "turnLEDsOff", "()V"));
    }

    void Modern_robotics_color_beacon::set_red() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon, env->GetMethodID(jclazz, "setRed", "()V"));
    }

    void Modern_robotics_color_beacon::set_green() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon, env->GetMethodID(jclazz, "setGreen", "()V"));
    }

    void Modern_robotics_color_beacon::set_yellow() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon,
                            env->GetMethodID(jclazz, "setYellow", "()V"));
    }

    void Modern_robotics_color_beacon::set_blue() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon, env->GetMethodID(jclazz, "setBlue", "()V"));
    }

    void Modern_robotics_color_beacon::set_purple() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon,
                            env->GetMethodID(jclazz, "setPurple", "()V"));
    }

    void Modern_robotics_color_beacon::set_teal() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon, env->GetMethodID(jclazz, "setTeal", "()V"));
    }

    void Modern_robotics_color_beacon::set_white() const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon, env->GetMethodID(jclazz, "setWhite", "()V"));
    }

    void Modern_robotics_color_beacon::set_color(const int &red, const int &green,
                                                 const int &blue) const {
        attach_thread
        env->CallVoidMethod(modernRoboticsColorBeacon,
                            env->GetMethodID(jclazz, "setColor", "(III)V"), static_cast<jint>(red),
                            static_cast<jint>(green), static_cast<jint>(blue));
    }

    bool Modern_robotics_color_beacon::do_initialize() const {
        attach_thread
        return env->CallBooleanMethod(modernRoboticsColorBeacon,
                                      env->GetMethodID(jclazz, "doInitialize", "()Z"));
    }

    std::string Modern_robotics_color_beacon::get_device_name() const {
        attach_thread
        auto jstr = (jstring) env->CallObjectMethod(modernRoboticsColorBeacon,
                                                    env->GetMethodID(jclazz, "getDeviceName",
                                                                     "()Ljava/lang/String;"));

        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        std::string str(cstr);
        env->DeleteLocalRef(jstr);

        return str;
    }
}