//
// Created by fnlg on 10.05.2024.
//

#include "hardware/Gyro_sensor.h"

namespace sdk {
    jclass Gyro_sensor::jclazz;

    Gyro_sensor::Gyro_sensor(const jobject &gyroSensor) : gyroSensor(gyroSensor) {}

    Gyro_sensor::~Gyro_sensor() {
        if (gyroSensor) {
            attach_thread
            env->DeleteGlobalRef(gyroSensor);
            gyroSensor = nullptr;
        }
    }

    Gyro_sensor &Gyro_sensor::operator=(const Gyro_sensor &gyro_sensor) {
        if (this != &gyro_sensor && gyro_sensor.gyroSensor) {
            attach_thread
            this->gyroSensor = env->NewGlobalRef(gyro_sensor.gyroSensor);
        }
        return *this;
    }

    Gyro_sensor &Gyro_sensor::operator=(const jobject &gyroSensor) {
        if (this->gyroSensor) {
            attach_thread
            env->DeleteGlobalRef(this->gyroSensor);
        }
        this->gyroSensor = gyroSensor;
        return *this;
    }


    void Gyro_sensor::calibrate() const {
        attach_thread
        env->CallVoidMethod(gyroSensor, env->GetMethodID(jclazz, "calibrate", "()V"));
    }

    bool Gyro_sensor::is_calibrating() const {
        attach_thread
        return env->CallBooleanMethod(gyroSensor,
                                      env->GetMethodID(jclazz, "isCalibrating", "()Z"));
    }

    int Gyro_sensor::get_heading() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(gyroSensor,
                                                   env->GetMethodID(jclazz, "getHeading",
                                                                    "()I")));
    }

    double Gyro_sensor::get_rotation_fraction() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(gyroSensor, env->GetMethodID(jclazz,
                                                                                      "getRotationFraction",
                                                                                      "()D")));
    }

    int Gyro_sensor::raw_x() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(gyroSensor,
                                                   env->GetMethodID(jclazz, "rawX", "()I")));
    }

    int Gyro_sensor::raw_y() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(gyroSensor,
                                                   env->GetMethodID(jclazz, "rawY", "()I")));
    }

    int Gyro_sensor::raw_z() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(gyroSensor,
                                                   env->GetMethodID(jclazz, "rawZ", "()I")));
    }

    void Gyro_sensor::reset_z_axis_integrator() const {
        attach_thread
        env->CallVoidMethod(gyroSensor,
                            env->GetMethodID(jclazz, "resetZAxisIntegrator", "()V"));
    }

    std::string Gyro_sensor::status() const {
        attach_thread
        auto jstr = (jstring) (env->CallObjectMethod(gyroSensor,
                                                     env->GetMethodID(jclazz, "status",
                                                                      "()Ljava/lang/String;")));

        const char *cStr = env->GetStringUTFChars(jstr, nullptr);
        std::string str(cStr);
        env->DeleteLocalRef(jstr);

        return str;
    }
} // sdk