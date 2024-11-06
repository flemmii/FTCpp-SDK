//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_CPPENV_H
#define FTCROBOTCONTROLLER_CPPENV_H

#define find_class_and_create_global_ref(name, globalRef) \
    localRef = env->FindClass(name);                    \
    sdk::globalRef = reinterpret_cast<jclass>(env->NewGlobalRef(localRef)); \
    env->DeleteLocalRef(localRef);                             \

// Macro to initialize the C++ environment for the FTC Robot Controller
// This includes getting the JavaVM, creating global references to various Java classes,
// and initializing the linear_op_mode namespace.
#define init_sdk \
    jclass localRef;             \
    env->GetJavaVM(&sdk::jvm);                                                                                               \
    sdk::linear_op_mode::thiz = env->NewGlobalRef(thiz);                                                                            \
    jclass local_jclazz = env->GetObjectClass(thiz);                                                                  \
    sdk::linear_op_mode::jclazz = reinterpret_cast<jclass>(env->NewGlobalRef(local_jclazz));                                    \
    env->DeleteLocalRef(local_jclazz);                                                                                 \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/Telemetry", telemetry::jclazz)                   \
    jobject localTelemetryObject = env->GetObjectField(sdk::linear_op_mode::thiz, env->GetFieldID(sdk::linear_op_mode::jclazz, "telemetry", "Lorg/firstinspires/ftc/robotcore/external/Telemetry;")); \
    sdk::telemetry::telemetry = env->NewGlobalRef(localTelemetryObject); \
    env->DeleteLocalRef(localTelemetryObject);                                                                               \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/Telemetry$DisplayFormat", telemetry::jclazz_DisplayFormat)      \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/AnalogInput", Analog_input::jclazz)                     \
    find_class_and_create_global_ref("com/qualcomm/hardware/bosch/BNO055IMU", BNO055IMU::jclazz)                             \
    find_class_and_create_global_ref("com/qualcomm/hardware/bosch/BNO055IMU$AngleUnit", BNO055IMU::jclazz_AngleUnit)         \
    find_class_and_create_global_ref("com/qualcomm/hardware/bosch/BNO055IMU$AccelUnit", BNO055IMU::jclazz_AccelUnit)         \
    find_class_and_create_global_ref("com/qualcomm/hardware/bosch/BNO055IMU$Parameters", BNO055IMU::Parameters::jclazz)      \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/ColorRangeSensor", Color_range_sensor::jclazz)            \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/ColorSensor", Color_sensor::jclazz)                    \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/CRServo", CR_servo::jclazz)                            \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DcMotor", Dc_motor::jclazz)                            \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DcMotor$RunMode", Dc_motor::jclazz_RunMode);                                    \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior", Dc_motor::jclazz_ZeroPowerBehavior); \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DcMotorEx", Dc_motor_ex::jclazz);                        \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DcMotorSimple", Dc_motor_simple::jclazz);                \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DcMotorSimple$Direction", Dc_motor_simple::jclazz_Direction);   \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DigitalChannel", Digital_channel::jclazz)              \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DigitalChannel$Mode", Digital_channel::jclazz_Mode)    \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/DistanceSensor", Distance_sensor::jclazz)              \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/navigation/DistanceUnit", Distance_sensor::DistanceUnit) \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/Gamepad", Gamepad::jclazz)                             \
    jobject localGamepad1 = env->GetObjectField(sdk::linear_op_mode::thiz, env->GetFieldID(sdk::linear_op_mode::jclazz, "gamepad1", "Lcom/qualcomm/robotcore/hardware/Gamepad;")); \
    sdk::gamepads::gamepad1 = env->NewGlobalRef(localGamepad1); \
    env->DeleteLocalRef(localGamepad1); \
    jobject localGamepad2 = env->GetObjectField(sdk::linear_op_mode::thiz, env->GetFieldID(sdk::linear_op_mode::jclazz, "gamepad2", "Lcom/qualcomm/robotcore/hardware/Gamepad;")); \
    sdk::gamepads::gamepad2 = env->NewGlobalRef(localGamepad2); \
    env->DeleteLocalRef(localGamepad2);                                                                                      \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/GyroSensor", Gyro_sensor::jclazz)                      \
    jobject localHardwareMap = env->GetObjectField(sdk::linear_op_mode::thiz, env->GetFieldID(sdk::linear_op_mode::jclazz, "hardwareMap", "Lcom/qualcomm/robotcore/hardware/HardwareMap;"));               \
    sdk::hardware_map::hardwareMap = env->NewGlobalRef(localHardwareMap);                                                    \
    env->DeleteLocalRef(localHardwareMap);                                                                                   \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/LightSensor", Light_sensor::jclazz)                    \
    find_class_and_create_global_ref("com/qualcomm/hardware/lynx/LynxModule", Lynx_module::jclazz)                                    \
    find_class_and_create_global_ref("com/qualcomm/hardware/lynx/LynxModule$BulkCachingMode", Lynx_module::jclazz_BulkCachingMode)  \
    find_class_and_create_global_ref("org/firstinspires/ftc/teamcode/sdk/hardware/ModernRoboticsColorBeacon", Modern_robotics_color_beacon::jclazz) \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/PIDFCoefficients", PIDF_coefficients::jclazz)          \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/Servo", Servo::jclazz)                                  \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/Servo$Direction", Servo::jclazz_Direction)             \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/ServoController", Servo_controller::jclazz)            \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/ServoController$PwmStatus", Servo_controller::jclazz_PwmStatus)                                                 \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/ServoImplEx", Servo_impl_ex::jclazz)             \
    find_class_and_create_global_ref("com/qualcomm/robotcore/hardware/TouchSensor", Touch_sensor::jclazz)                                           \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/hardware/camera/CameraName", Camera_name::jclazz)    \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/hardware/camera/WebcamName", Webcam_name::jclazz)    \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/navigation/AngleUnit", AngleUnit)             \
    find_class_and_create_global_ref("org/firstinspires/ftc/robotcore/external/navigation/VoltageUnit", VoltageUnit)         \
    find_class_and_create_global_ref("org/firstinspires/ftc/vision/VisionPortal", Vision_portal::jclazz)                     \
    find_class_and_create_global_ref("org/firstinspires/ftc/vision/VisionPortal$StreamFormat", Vision_portal::jclazz_StreamFormat)                                                 \
    find_class_and_create_global_ref("org/firstinspires/ftc/vision/VisionPortal$MultiPortalLayout", Vision_portal::jclazz_MultiPortalLayout)                                          \
    find_class_and_create_global_ref("org/firstinspires/ftc/vision/VisionPortal$Builder", Vision_portal::Builder::jclazz) \
    find_class_and_create_global_ref("org/firstinspires/ftc/teamcode/sdk/VisionProcessor/FirstVisionProcessor", vision_processor::first_vision_processor::jclazz)              \
    find_class_and_create_global_ref("org/firstinspires/ftc/teamcode/sdk/VisionProcessor/SecondVisionProcessor", vision_processor::second_vision_processor::jclazz)                                                                        \

#endif //FTCROBOTCONTROLLER_CPPENV_H