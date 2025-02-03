//
// Created by flemmi on 03.02.25.
//

#include "hardware/GoBilda_pinpoint_driver.h"

namespace sdk {
    jclass GoBilda_pinpoint_driver::jclazz;
    jclass GoBilda_pinpoint_driver::jclazz_DeviceStatus;
    jclass GoBilda_pinpoint_driver::jclazz_readData;
    jclass GoBilda_pinpoint_driver::jclazz_EncoderDirection;
    jclass GoBilda_pinpoint_driver::jclazz_GoBildaOdometryPods;

    GoBilda_pinpoint_driver::GoBilda_pinpoint_driver(const jobject &goBildaPinpointDriver) : goBildaPinpointDriver(
            goBildaPinpointDriver) {}

    GoBilda_pinpoint_driver::~GoBilda_pinpoint_driver() {
        if (goBildaPinpointDriver) {
            attach_thread
            env->DeleteGlobalRef(goBildaPinpointDriver);
            goBildaPinpointDriver = nullptr;
        }
    }

    GoBilda_pinpoint_driver &GoBilda_pinpoint_driver::operator=(const GoBilda_pinpoint_driver &goBildaPinpointDriver) {
        if (this != &goBildaPinpointDriver &&
            goBildaPinpointDriver.goBildaPinpointDriver) {
            attach_thread
            this->goBildaPinpointDriver = env->NewGlobalRef(
                    goBildaPinpointDriver.goBildaPinpointDriver);
        }
        return *this;
    }

    GoBilda_pinpoint_driver &GoBilda_pinpoint_driver::operator=(const jobject &goBildaPinpointDriver) {
        if (this->goBildaPinpointDriver) {
            attach_thread
            env->DeleteGlobalRef(this->goBildaPinpointDriver);
        }
        this->goBildaPinpointDriver = goBildaPinpointDriver;
        return *this;
    }

    bool GoBilda_pinpoint_driver::do_initialize() const {
        attach_thread
        return env->CallBooleanMethod(goBildaPinpointDriver,
                                      env->GetMethodID(jclazz, "doInitialize", "()Z"));
    }

    std::string GoBilda_pinpoint_driver::get_device_name() const {
        attach_thread
        jstring jstr = (jstring) env->CallObjectMethod(goBildaPinpointDriver,
                                                       env->GetMethodID(jclazz, "getDeviceName",
                                                                       "()Ljava/lang/String;"));

        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        std::string str(cstr);
        env->DeleteLocalRef(jstr);
        return str;
    }

    void GoBilda_pinpoint_driver::update() const {
        attach_thread
        env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "update", "()V"));
    }

    void GoBilda_pinpoint_driver::update(const read_data &data) const {
        attach_thread
        jobject readData = env->GetStaticObjectField(jclazz_readData,
                                                    env->GetStaticFieldID(jclazz_readData,
                                                                          read_data_to_string(
                                                                                  data),
                                                                          "Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$readData;"));
        env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "update", "(Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$readData;)V"), readData);

        env->DeleteLocalRef(readData);
    }

    void GoBilda_pinpoint_driver::set_offsets(double x_offset, double y_offset) const {
        attach_thread
        env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setOffsets", "(DD)V"),
                            static_cast<jdouble>(x_offset), static_cast<jdouble>(y_offset));
    }

    int GoBilda_pinpoint_driver::get_device_ID() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getDeviceID", "()I")));
    }

    int GoBilda_pinpoint_driver::get_device_version() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getDeviceVersion", "()I")));
    }

    float GoBilda_pinpoint_driver::get_yaw_scalar() const {
        attach_thread
        return static_cast<float>(env->CallFloatMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getYawScalar", "()F")));
    }

    GoBilda_pinpoint_driver::Device_status GoBilda_pinpoint_driver::get_device_status() const {
        attach_thread
        jobject deviceStatus = env->CallObjectMethod(goBildaPinpointDriver,
                                                    env->GetMethodID(jclazz, "getDeviceStatus",
                                                                     "()Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$DeviceStatus;"));

        jstring name = (jstring) env->CallObjectMethod(deviceStatus,
                                                      env->GetMethodID(jclazz_DeviceStatus,
                                                                       "name",
                                                                       "()Ljava/lang/String;"));

        const char *deviceStatusName = env->GetStringUTFChars(name, nullptr);
        std::string strDeviceStatusName(deviceStatusName);
        env->DeleteLocalRef(name);
        env->DeleteLocalRef(deviceStatus);

        if (strDeviceStatusName == "NOT_READY")
            return Device_status::NOT_READY;
        else if (strDeviceStatusName == "READY")
            return Device_status::READY;
        else if (strDeviceStatusName == "CALIBRATING")
            return Device_status::CALIBRATING;
        else if (strDeviceStatusName == "FAULT_X_POD_NOT_DETECTED")
            return Device_status::FAULT_X_POD_NOT_DETECTED;
        else if (strDeviceStatusName == "FAULT_Y_POD_NOT_DETECTED")
            return Device_status::FAULT_Y_POD_NOT_DETECTED;
        else if (strDeviceStatusName == "FAULT_NO_PODS_DETECTED")
            return Device_status::FAULT_NO_PODS_DETECTED;
        else if (strDeviceStatusName == "FAULT_IMU_RUNAWAY")
            return Device_status::FAULT_IMU_RUNAWAY;
    }

    int GoBilda_pinpoint_driver::get_loop_time() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getLoopTime", "()I")));
    }

    double GoBilda_pinpoint_driver::get_frequency() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getFrequency", "()D")));
    }

    int GoBilda_pinpoint_driver::get_encoder_x() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getEncoderX", "()I")));
    }

    int GoBilda_pinpoint_driver::get_encoder_y() const {
        attach_thread
        return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getEncoderY", "()I")));
    }

    double GoBilda_pinpoint_driver::get_pos_x() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getPosX", "()D")));
    }

    double GoBilda_pinpoint_driver::get_pos_y() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getPosY", "()D")));
    }

    double GoBilda_pinpoint_driver::get_heading() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getHeading", "()D")));
    }

    double GoBilda_pinpoint_driver::get_vel_x() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getVelX", "()D")));
    }

    double GoBilda_pinpoint_driver::get_vel_y() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getVelY", "()D")));
    }

    double GoBilda_pinpoint_driver::get_heading_velocity() const {
        attach_thread
        return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getHeadingVelocity", "()D")));
    }

    float GoBilda_pinpoint_driver::get_x_offset() const {
        attach_thread
        return static_cast<float>(env->CallFloatMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getXOffset", "()F")));
    }

    float GoBilda_pinpoint_driver::get_y_offset() const {
        attach_thread
        return static_cast<float>(env->CallFloatMethod(goBildaPinpointDriver,
                                 env->GetMethodID(jclazz, "getYOffset", "()F")));
    }    
}