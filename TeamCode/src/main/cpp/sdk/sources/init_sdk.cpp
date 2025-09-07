#include "init_sdk.hpp"

namespace sdk
{
    jclass Angle_unit::jclazz;
    jclass Distance_unit::jclazz;
    jclass Pose2D::jclazz;

    extern "C" void init_sdk(JNIEnv *env, jobject thiz)
    {
        jclass localRef;
        env->GetJavaVM(&jvm);
        linear_op_mode::thiz = env->NewGlobalRef(thiz);
        jclass local_jclazz = env->GetObjectClass(thiz);
        linear_op_mode::jclazz = reinterpret_cast<jclass>(env->NewGlobalRef(local_jclazz));
        env->DeleteLocalRef(local_jclazz);

        telemetry::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/Telemetry");
        jobject localTelemetryObject = env->GetObjectField(linear_op_mode::thiz, env->GetFieldID(linear_op_mode::jclazz, "telemetry", "Lorg/firstinspires/ftc/robotcore/external/Telemetry;"));
        telemetry::telemetry = env->NewGlobalRef(localTelemetryObject);
        env->DeleteLocalRef(localTelemetryObject);

        telemetry::jclazz_DisplayFormat = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/Telemetry$DisplayFormat");
        Analog_input::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/AnalogInput");
        BNO055IMU::jclazz = find_and_create_global_ref(env, "com/qualcomm/hardware/bosch/BNO055IMU");
        BNO055IMU::jclazz_AngleUnit = find_and_create_global_ref(env, "com/qualcomm/hardware/bosch/BNO055IMU$AngleUnit");
        BNO055IMU::jclazz_AccelUnit = find_and_create_global_ref(env, "com/qualcomm/hardware/bosch/BNO055IMU$AccelUnit");
        BNO055IMU::Parameters::jclazz = find_and_create_global_ref(env, "com/qualcomm/hardware/bosch/BNO055IMU$Parameters");
        Color_range_sensor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/ColorRangeSensor");
        Color_sensor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/ColorSensor");
        CR_servo::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/CRServo");
        Dc_motor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DcMotor");
        Dc_motor::jclazz_RunMode = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DcMotor$RunMode");
        Dc_motor::jclazz_ZeroPowerBehavior = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior");
        Dc_motor_ex::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DcMotorEx");
        Dc_motor_ex::CurrentUnit = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/navigation/CurrentUnit");
        Dc_motor_simple::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DcMotorSimple");
        Dc_motor_simple::jclazz_Direction = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DcMotorSimple$Direction");
        Digital_channel::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DigitalChannel");
        Digital_channel::jclazz_Mode = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DigitalChannel$Mode");
        Distance_sensor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/DistanceSensor");
        Distance_unit::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/navigation/DistanceUnit");
        Gamepad::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/Gamepad");

        jobject localGamepad1 = env->GetObjectField(linear_op_mode::thiz, env->GetFieldID(linear_op_mode::jclazz, "gamepad1", "Lcom/qualcomm/robotcore/hardware/Gamepad;"));
        gamepads::gamepad1 = env->NewGlobalRef(localGamepad1);
        env->DeleteLocalRef(localGamepad1);

        jobject localGamepad2 = env->GetObjectField(linear_op_mode::thiz, env->GetFieldID(linear_op_mode::jclazz, "gamepad2", "Lcom/qualcomm/robotcore/hardware/Gamepad;"));
        gamepads::gamepad2 = env->NewGlobalRef(localGamepad2);
        env->DeleteLocalRef(localGamepad2);

        Gyro_sensor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/GyroSensor");
        GoBilda_pinpoint_driver::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver");
        GoBilda_pinpoint_driver::jclazz_DeviceStatus = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$DeviceStatus");
        GoBilda_pinpoint_driver::jclazz_EncoderDirection = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$EncoderDirection");
        GoBilda_pinpoint_driver::jclazz_GoBildaOdometryPods = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$GoBildaOdometryPods");
        GoBilda_pinpoint_driver::jclazz_readData = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/hardware/GoBildaPinpointDriver$readData");

        jobject localHardwareMap = env->GetObjectField(linear_op_mode::thiz, env->GetFieldID(linear_op_mode::jclazz, "hardwareMap", "Lcom/qualcomm/robotcore/hardware/HardwareMap;"));
        hardware_map::hardwareMap = env->NewGlobalRef(localHardwareMap);
        env->DeleteLocalRef(localHardwareMap);

        Light_sensor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/LightSensor");
        Lynx_module::jclazz = find_and_create_global_ref(env, "com/qualcomm/hardware/lynx/LynxModule");
        Lynx_module::jclazz_BulkCachingMode = find_and_create_global_ref(env, "com/qualcomm/hardware/lynx/LynxModule$BulkCachingMode");
        Modern_robotics_color_beacon::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/hardware/ModernRoboticsColorBeacon");
        PIDF_coefficients::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/PIDFCoefficients");
        Pwm_control::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/PwmControl");
        Pwm_control::Pwm_range::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/PwmControl$PwmRange");
        Servo::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/Servo");
        Servo::jclazz_Direction = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/Servo$Direction");
        Servo_controller::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/ServoController");
        Servo_controller::jclazz_PwmStatus = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/ServoController$PwmStatus");
        Servo_impl_ex::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/ServoImplEx");
        Touch_sensor::jclazz = find_and_create_global_ref(env, "com/qualcomm/robotcore/hardware/TouchSensor");
        Camera_name::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/hardware/camera/CameraName");
        Webcam_name::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/hardware/camera/WebcamName");
        Angle_unit::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/navigation/AngleUnit");
        Distance_unit::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/navigation/DistanceUnit");
        Pose2D::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/navigation/Pose2D");
        VoltageUnit = find_and_create_global_ref(env, "org/firstinspires/ftc/robotcore/external/navigation/VoltageUnit");
        Vision_portal::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/vision/VisionPortal");
        Vision_portal::jclazz_StreamFormat = find_and_create_global_ref(env, "org/firstinspires/ftc/vision/VisionPortal$StreamFormat");
        Vision_portal::jclazz_MultiPortalLayout = find_and_create_global_ref(env, "org/firstinspires/ftc/vision/VisionPortal$MultiPortalLayout");
        Vision_portal::Builder::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/vision/VisionPortal$Builder");
        vision_processor::first_vision_processor::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/VisionProcessor/FirstVisionProcessor");
        vision_processor::second_vision_processor::jclazz = find_and_create_global_ref(env, "org/firstinspires/ftc/teamcode/sdk/VisionProcessor/SecondVisionProcessor");

        vision_processor::load_native_methods();

        // Redirect std::cout to logcat
        std::cout.rdbuf(new LogcatBuffer(ANDROID_LOG_INFO, "Teamcode"));
        std::cerr.rdbuf(new LogcatBuffer(ANDROID_LOG_ERROR, "Teamcode"));
        std::clog.rdbuf(new LogcatBuffer(ANDROID_LOG_DEBUG, "Teamcode"));
    }
}