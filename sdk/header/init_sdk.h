//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_CPPENV_H
#define FTCROBOTCONTROLLER_CPPENV_H

// Macro to initialize the C++ environment for the FTC Robot Controller
// This includes getting the JavaVM, creating global references to various Java classes,
// and initializing the linear_op_mode namespace.
#define init_sdk        env->GetJavaVM(&sdk::jvm);                                                                                                                                   \
                        sdk::currentOpMode = env->NewGlobalRef(thiz);                                                                                                                \
                        sdk::CurrentOpMode = reinterpret_cast<jclass>(env->NewGlobalRef(env->GetObjectClass(thiz)));                                                                 \
                        sdk::DcMotorEx = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DcMotorEx")));                                   \
                        sdk::DcMotor_RunMode = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$RunMode")));                               \
                        sdk::PIDFCoefficients = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/PIDFCoefficients")));                     \
                        sdk::DcMotor_ZeroPowerBehavior = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior")));           \
                        sdk::DcMotorSimple = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple$Direction")));                                                                                                                                                             \
                        sdk::DcMotorSimple_Direction = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple")));                 \
                        sdk::Gamepad = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/Gamepad")));                                       \
                        sdk::Telemetry = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/robotcore/external/Telemetry")));                          \
                        sdk::LynxModule = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/hardware/lynx/LynxModule")));                                      \
                        sdk::BulkCachingMode = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/hardware/lynx/LynxModule$BulkCachingMode")));                 \
                        sdk::BNO055IMU = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU")));                                       \
                        sdk::Parameters = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$Parameters")));                           \
                        sdk::AngleUnit = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$AngleUnit")));                             \
                        sdk::AccelUnit = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$AccelUnit")));                             \
                        sdk::DistanceSensor = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DistanceSensor")));                         \
                        sdk::DistanceUnit = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/DistanceUnit")));         \
                        sdk::Servo = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/Servo")));                                           \
                        sdk::Servo_Direction = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/Servo$Direction")));                       \
                        sdk::ServoController = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/ServoController")));                       \
                        sdk::ServoController_PwmStatus = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/ServoController$PwmStatus")));   \
                        sdk::voltage_unit::VoltageUnit = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/VoltageUnit")));           \
                        sdk::VisionPortal = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/vision/VisionPortal")));                                \
                        sdk::VisionPortal_Builder = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/vision/VisionPortal$Builder")));                \
                        sdk::ColorSensor = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/ColorSensor")));                               \
                        sdk::TouchSensor = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/TouchSensor")));                               \
                        sdk::AnalogInput = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/AnalogInput")));                               \
                        sdk::DigitalChannel = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("com/qualcomm/robotcore/hardware/DigitalChannel")));                         \
                        vision_processor::FirstVisionProcessor = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/teamcode/VisionProcessor/FirstVisionProcessor"))); \
                        vision_processor::SecondVisionProcessor = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("org/firstinspires/ftc/teamcode/VisionProcessor/SecondVisionProcessor"))); \
                        sdk::linear_op_mode::init_cpp();                                                                                                                             \

#endif //FTCROBOTCONTROLLER_CPPENV_H