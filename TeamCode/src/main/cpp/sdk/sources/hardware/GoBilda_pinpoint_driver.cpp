//
// Created by flemmi on 03.02.25.
//

#include "hardware/GoBilda_pinpoint_driver.hpp"

namespace sdk
{
    jclass GoBilda_pinpoint_driver::jclazz;
    jclass GoBilda_pinpoint_driver::jclazz_DeviceStatus;
    jclass GoBilda_pinpoint_driver::jclazz_readData;
    jclass GoBilda_pinpoint_driver::jclazz_EncoderDirection;
    jclass GoBilda_pinpoint_driver::jclazz_GoBildaOdometryPods;

    GoBilda_pinpoint_driver::GoBilda_pinpoint_driver(const jobject &goBildaPinpointDriver) : goBildaPinpointDriver(
                                                                                                 goBildaPinpointDriver) {}

    GoBilda_pinpoint_driver::~GoBilda_pinpoint_driver()
    {
        if (goBildaPinpointDriver)
        {
            attach_thread
                env->DeleteGlobalRef(goBildaPinpointDriver);
            goBildaPinpointDriver = nullptr;
        }
    }

    GoBilda_pinpoint_driver &GoBilda_pinpoint_driver::operator=(const GoBilda_pinpoint_driver &goBildaPinpointDriver)
    {
        if (this != &goBildaPinpointDriver &&
            goBildaPinpointDriver.goBildaPinpointDriver)
        {
            attach_thread this->goBildaPinpointDriver = env->NewGlobalRef(
                goBildaPinpointDriver.goBildaPinpointDriver);
        }
        return *this;
    }

    GoBilda_pinpoint_driver &GoBilda_pinpoint_driver::operator=(const jobject &goBildaPinpointDriver)
    {
        if (this->goBildaPinpointDriver)
        {
            attach_thread
                env->DeleteGlobalRef(this->goBildaPinpointDriver);
        }
        this->goBildaPinpointDriver = goBildaPinpointDriver;
        return *this;
    }

    bool GoBilda_pinpoint_driver::do_initialize() const
    {
        attach_thread return env->CallBooleanMethod(goBildaPinpointDriver,
                                                    env->GetMethodID(jclazz, "doInitialize", "()Z"));
    }

    std::string GoBilda_pinpoint_driver::get_device_name() const
    {
        attach_thread
            jstring jstr = (jstring)env->CallObjectMethod(goBildaPinpointDriver,
                                                          env->GetMethodID(jclazz, "getDeviceName",
                                                                           "()Ljava/lang/String;"));

        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        std::string str(cstr);
        env->DeleteLocalRef(jstr);
        return str;
    }

    void GoBilda_pinpoint_driver::update()
    {
        attach_thread
            env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "update", "()V"));
    }

    void GoBilda_pinpoint_driver::update(const read_data &data) const
    {
        attach_thread
            jobject readData = env->GetStaticObjectField(jclazz_readData,
                                                         env->GetStaticFieldID(jclazz_readData,
                                                                               read_data_to_string(
                                                                                   data),
                                                                               "Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$readData;"));
        env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "update", "(Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$readData;)V"), readData);

        env->DeleteLocalRef(readData);
    }

    void GoBilda_pinpoint_driver::set_offsets(const double &x_offset, const double &y_offset) const
    {
        attach_thread
            env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setOffsets", "(DD)V"),
                                static_cast<jdouble>(x_offset), static_cast<jdouble>(y_offset));
    }

    void GoBilda_pinpoint_driver::recalibrate_IMU() const
    {
        attach_thread
            env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "recalibrateIMU", "()V"));
    }

    void GoBilda_pinpoint_driver::reset_pos_and_IMU() const
    {
        attach_thread
            env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "resetPosAndIMU", "()V"));
    }

    void GoBilda_pinpoint_driver::set_encoder_resolution(const GoBilda_odometry_pods &pods) const
    {
        attach_thread
            jobject goBildaOdometryPods = env->GetStaticObjectField(jclazz_GoBildaOdometryPods,
                                                                    env->GetStaticFieldID(jclazz_GoBildaOdometryPods,
                                                                                          goBilda_odometry_pods_to_string(
                                                                                              pods),
                                                                                          "Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$GoBildaOdometryPods;"));
        env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setEncoderResolution", "(Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$GoBildaOdometryPods;)V"), goBildaOdometryPods);

        env->DeleteLocalRef(goBildaOdometryPods);
    }

    void GoBilda_pinpoint_driver::set_encoder_resolution(const double &ticks_per_mm) const
    {
        attach_thread
            env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setEncoderResolution", "(D)V"),
                                static_cast<jdouble>(ticks_per_mm));
    }

    void GoBilda_pinpoint_driver::set_yaw_scalar(const double &yaw_offset) const
    {
        attach_thread
            env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setYawScalar", "(D)V"),
                                static_cast<jdouble>(yaw_offset));
    }

    void GoBilda_pinpoint_driver::set_position(const Pose2D &pos) const
    {
        attach_thread
            jobject distanceUnit = env->GetStaticObjectField(Distance_unit::jclazz,
                                                             env->GetStaticFieldID(Distance_unit::jclazz,
                                                                                   "MM",
                                                                                   "Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;"));

        jobject angleUnit = env->GetStaticObjectField(Angle_unit::jclazz,
                                                      env->GetStaticFieldID(Angle_unit::jclazz,
                                                                            "DEGREES",
                                                                            "Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;"));
        jobject pose2D = env->NewObject(Pose2D::jclazz,
                                        env->GetMethodID(Pose2D::jclazz, "<init>", "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;DDLorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;D)V"),
                                        distanceUnit, static_cast<jdouble>(pos.getX(Distance_unit::MM)), static_cast<jdouble>(pos.getY(Distance_unit::MM)), angleUnit, static_cast<jdouble>(pos.getHeading(Angle_unit::DEGREES)));
        env->DeleteLocalRef(env->CallObjectMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setPosition", "(Lorg/firstinspires/ftc/robotcore/external/navigation/Pose2D;)Lorg/firstinspires/ftc/robotcore/external/navigation/Pose2D;"), pose2D));

        env->DeleteLocalRef(pose2D);
        env->DeleteLocalRef(distanceUnit);
        env->DeleteLocalRef(angleUnit);
    }

    void GoBilda_pinpoint_driver::set_encoder_directions(const Encoder_direction &x_encoder,
                                                         const Encoder_direction &y_encoder) const
    {
        attach_thread
            jobject xEncoderDirection = env->GetStaticObjectField(jclazz_EncoderDirection,
                                                                  env->GetStaticFieldID(jclazz_EncoderDirection,
                                                                                        encoder_direction_to_string(
                                                                                            x_encoder),
                                                                                        "Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$EncoderDirection;"));
        jobject yEncoderDirection = env->GetStaticObjectField(jclazz_EncoderDirection,
                                                              env->GetStaticFieldID(jclazz_EncoderDirection,
                                                                                    encoder_direction_to_string(
                                                                                        y_encoder),
                                                                                    "Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$EncoderDirection;"));

        env->CallVoidMethod(goBildaPinpointDriver, env->GetMethodID(jclazz, "setEncoderDirections", "(Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$EncoderDirection;Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$EncoderDirection;)V"),
                            xEncoderDirection, yEncoderDirection);

        env->DeleteLocalRef(xEncoderDirection);
        env->DeleteLocalRef(yEncoderDirection);
    }

    int GoBilda_pinpoint_driver::get_device_ID() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                                                 env->GetMethodID(jclazz, "getDeviceID", "()I")));
    }

    int GoBilda_pinpoint_driver::get_device_version() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                                                 env->GetMethodID(jclazz, "getDeviceVersion", "()I")));
    }

    float GoBilda_pinpoint_driver::get_yaw_scalar() const
    {
        attach_thread return static_cast<float>(env->CallFloatMethod(goBildaPinpointDriver,
                                                                     env->GetMethodID(jclazz, "getYawScalar", "()F")));
    }

    GoBilda_pinpoint_driver::Device_status GoBilda_pinpoint_driver::get_device_status() const
    {
        attach_thread
            jobject deviceStatus = env->CallObjectMethod(goBildaPinpointDriver,
                                                         env->GetMethodID(jclazz, "getDeviceStatus",
                                                                          "()Lorg/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$DeviceStatus;"));

        jstring name = (jstring)env->CallObjectMethod(deviceStatus,
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
        else
            return Device_status::FAULT_IMU_RUNAWAY;
    }

    int GoBilda_pinpoint_driver::get_loop_time() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                                                 env->GetMethodID(jclazz, "getLoopTime", "()I")));
    }

    double GoBilda_pinpoint_driver::get_frequency() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getFrequency", "()D")));
    }

    int GoBilda_pinpoint_driver::get_encoder_x() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                                                 env->GetMethodID(jclazz, "getEncoderX", "()I")));
    }

    int GoBilda_pinpoint_driver::get_encoder_y() const
    {
        attach_thread return static_cast<int>(env->CallIntMethod(goBildaPinpointDriver,
                                                                 env->GetMethodID(jclazz, "getEncoderY", "()I")));
    }

    double GoBilda_pinpoint_driver::get_pos_x() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getPosX", "()D")));
    }

    double GoBilda_pinpoint_driver::get_pos_y() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getPosY", "()D")));
    }

    double GoBilda_pinpoint_driver::get_heading() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getHeading", "()D")));
    }

    double GoBilda_pinpoint_driver::get_vel_x() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getVelX", "()D")));
    }

    double GoBilda_pinpoint_driver::get_vel_y() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getVelY", "()D")));
    }

    double GoBilda_pinpoint_driver::get_heading_velocity() const
    {
        attach_thread return static_cast<double>(env->CallDoubleMethod(goBildaPinpointDriver,
                                                                       env->GetMethodID(jclazz, "getHeadingVelocity", "()D")));
    }

    float GoBilda_pinpoint_driver::get_x_offset() const
    {
        attach_thread return static_cast<float>(env->CallFloatMethod(goBildaPinpointDriver,
                                                                     env->GetMethodID(jclazz, "getXOffset", "()F")));
    }

    float GoBilda_pinpoint_driver::get_y_offset() const
    {
        attach_thread return static_cast<float>(env->CallFloatMethod(goBildaPinpointDriver,
                                                                     env->GetMethodID(jclazz, "getYOffset", "()F")));
    }

    Pose2D GoBilda_pinpoint_driver::get_position() const
    {
        attach_thread
            jobject pose2D = env->CallObjectMethod(goBildaPinpointDriver,
                                                   env->GetMethodID(jclazz, "getPosition",
                                                                    "()Lorg/firstinspires/ftc/robotcore/external/navigation/Pose2D;"));

        jobject distanceUnit = env->GetStaticObjectField(Distance_unit::jclazz,
                                                         env->GetStaticFieldID(Distance_unit::jclazz,
                                                                               "MM",
                                                                               "Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;"));

        jobject angleUnit = env->GetStaticObjectField(Angle_unit::jclazz,
                                                      env->GetStaticFieldID(Angle_unit::jclazz,
                                                                            "DEGREES",
                                                                            "Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;"));

        jdouble x = env->CallDoubleMethod(pose2D,
                                          env->GetMethodID(Pose2D::jclazz, "getX", "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                          distanceUnit);
        jdouble y = env->CallDoubleMethod(pose2D,
                                          env->GetMethodID(Pose2D::jclazz, "getY", "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                          distanceUnit);
        jdouble heading = env->CallDoubleMethod(pose2D,
                                                env->GetMethodID(Pose2D::jclazz, "getHeading", "(Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;)D"),
                                                angleUnit);

        env->DeleteLocalRef(pose2D);
        env->DeleteLocalRef(distanceUnit);
        env->DeleteLocalRef(angleUnit);

        return {Distance_unit::MM, static_cast<double>(x), static_cast<double>(y), Angle_unit::DEGREES, static_cast<double>(heading)};
    }

    Pose2D GoBilda_pinpoint_driver::get_velocity() const
    {
        attach_thread
            jobject pose2D = env->CallObjectMethod(goBildaPinpointDriver,
                                                   env->GetMethodID(jclazz, "getVelocity",
                                                                    "()Lorg/firstinspires/ftc/robotcore/external/navigation/Pose2D;"));

        jobject distanceUnit = env->GetStaticObjectField(Distance_unit::jclazz,
                                                         env->GetStaticFieldID(Distance_unit::jclazz,
                                                                               "MM",
                                                                               "Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;"));

        jobject angleUnit = env->GetStaticObjectField(Angle_unit::jclazz,
                                                      env->GetStaticFieldID(Angle_unit::jclazz,
                                                                            "DEGREES",
                                                                            "Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;"));

        jdouble x = env->CallDoubleMethod(pose2D,
                                          env->GetMethodID(Pose2D::jclazz, "getX", "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                          distanceUnit);
        jdouble y = env->CallDoubleMethod(pose2D,
                                          env->GetMethodID(Pose2D::jclazz, "getY", "(Lorg/firstinspires/ftc/robotcore/external/navigation/DistanceUnit;)D"),
                                          distanceUnit);
        jdouble heading = env->CallDoubleMethod(pose2D,
                                                env->GetMethodID(Pose2D::jclazz, "getHeading", "(Lorg/firstinspires/ftc/robotcore/external/navigation/AngleUnit;)D"),
                                                angleUnit);

        env->DeleteLocalRef(pose2D);
        env->DeleteLocalRef(distanceUnit);
        env->DeleteLocalRef(angleUnit);

        return {Distance_unit::MM, static_cast<double>(x), static_cast<double>(y), Angle_unit::DEGREES, static_cast<double>(heading)};
    }
}