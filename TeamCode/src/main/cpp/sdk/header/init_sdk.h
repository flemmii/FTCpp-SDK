//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_CPPENV_H
#define FTCROBOTCONTROLLER_CPPENV_H

// Macro to initialize the C++ environment for the FTC Robot Controller
// This includes getting the JavaVM, creating global references to various Java classes,
// and initializing the linear_op_mode namespace.
#define init_sdk                                                                                                             \
    env->GetJavaVM(&sdk::jvm);                                                                                               \
    sdk::currentOpMode = env->NewGlobalRef(thiz);                                                                            \
    jobject localCurrentOpMode = env->GetObjectClass(thiz);                                                                  \
    sdk::CurrentOpMode = reinterpret_cast<jclass>(env->NewGlobalRef(localCurrentOpMode));                                    \
    env->DeleteLocalRef(localCurrentOpMode);                                                                                 \
    jobject localDcMotorEx = env->FindClass("com/qualcomm/robotcore/hardware/DcMotorEx");                                    \
    sdk::DcMotorEx = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotorEx));                                            \
    env->DeleteLocalRef(localDcMotorEx);                                                                                     \
    jobject localDcMotor_RunMode = env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$RunMode");                        \
    sdk::DcMotor_RunMode = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotor_RunMode));                                \
    env->DeleteLocalRef(localDcMotor_RunMode);                                                                               \
    jobject localPIDFCoefficients = env->FindClass("com/qualcomm/robotcore/hardware/PIDFCoefficients");                      \
    sdk::PIDFCoefficients = reinterpret_cast<jclass>(env->NewGlobalRef(localPIDFCoefficients));                              \
    env->DeleteLocalRef(localPIDFCoefficients);                                                                              \
    jobject localDcMotor_ZeroPowerBehavior = env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior");    \
    sdk::DcMotor_ZeroPowerBehavior = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotor_ZeroPowerBehavior));            \
    env->DeleteLocalRef(localDcMotor_ZeroPowerBehavior);                                                                     \
    jobject localDcMotorSimple = env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple$Direction");                  \
    sdk::DcMotorSimple = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotorSimple));                                    \
    env->DeleteLocalRef(localDcMotorSimple);                                                                                 \
    jobject localDcMotorSimple_Direction = env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple");                  \
    sdk::DcMotorSimple_Direction = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotorSimple_Direction));                \
    env->DeleteLocalRef(localDcMotorSimple_Direction);                                                                       \
    jobject localGamepad = env->FindClass("com/qualcomm/robotcore/hardware/Gamepad");                                        \
    sdk::Gamepad = reinterpret_cast<jclass>(env->NewGlobalRef(localGamepad));                                                \
    env->DeleteLocalRef(localGamepad);                                                                                       \
    jobject localTelemetry = env->FindClass("org/firstinspires/ftc/robotcore/external/Telemetry");                           \
    sdk::Telemetry = reinterpret_cast<jclass>(env->NewGlobalRef(localTelemetry));                                            \
    env->DeleteLocalRef(localTelemetry);                                                                                     \
    jobject localLynxModule = env->FindClass("com/qualcomm/hardware/lynx/LynxModule");                                       \
    sdk::LynxModule = reinterpret_cast<jclass>(env->NewGlobalRef(localLynxModule));                                          \
    env->DeleteLocalRef(localLynxModule);                                                                                    \
    jobject localBulkCachingMode = env->FindClass("com/qualcomm/hardware/lynx/LynxModule$BulkCachingMode");                  \
    sdk::BulkCachingMode = reinterpret_cast<jclass>(env->NewGlobalRef(localBulkCachingMode));                                \
    env->DeleteLocalRef(localBulkCachingMode);                                                                               \
    jobject localBNO055IMU = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU");                                        \
    sdk::BNO055IMU = reinterpret_cast<jclass>(env->NewGlobalRef(localBNO055IMU));                                            \
    env->DeleteLocalRef(localBNO055IMU);                                                                                     \
    jobject localParameters = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$Parameters");                            \
    sdk::Parameters = reinterpret_cast<jclass>(env->NewGlobalRef(localParameters));                                          \
    env->DeleteLocalRef(localParameters);                                                                                    \
    jobject localAngleUnit = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$AngleUnit");                              \
    sdk::AngleUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localAngleUnit));                                            \
    env->DeleteLocalRef(localAngleUnit);                                                                                     \
    jobject localAccelUnit = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$AccelUnit");                              \
    sdk::AccelUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localAccelUnit));                                            \
    env->DeleteLocalRef(localAccelUnit);                                                                                     \
    jobject localDistanceSensor = env->FindClass("com/qualcomm/robotcore/hardware/DistanceSensor");                          \
    sdk::DistanceSensor = reinterpret_cast<jclass>(env->NewGlobalRef(localDistanceSensor));                                  \
    env->DeleteLocalRef(localDistanceSensor);                                                                                \
    jobject localDistanceUnit = env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/DistanceUnit");          \
    sdk::DistanceUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localDistanceUnit));                                      \
    env->DeleteLocalRef(localDistanceUnit);                                                                                  \
    jobject localServo = env->FindClass("com/qualcomm/robotcore/hardware/Servo");                                            \
    sdk::Servo = reinterpret_cast<jclass>(env->NewGlobalRef(localServo));                                                    \
    env->DeleteLocalRef(localServo);                                                                                         \
    jobject localServo_Direction = env->FindClass("com/qualcomm/robotcore/hardware/Servo$Direction");                        \
    sdk::Servo_Direction = reinterpret_cast<jclass>(env->NewGlobalRef(localServo_Direction));                                \
    env->DeleteLocalRef(localServo_Direction);                                                                               \
    jobject localServoController = env->FindClass("com/qualcomm/robotcore/hardware/ServoController");                        \
    sdk::ServoController = reinterpret_cast<jclass>(env->NewGlobalRef(localServoController));                                \
    env->DeleteLocalRef(localServoController);                                                                               \
    jobject localServoController_PwmStatus = env->FindClass("com/qualcomm/robotcore/hardware/ServoController$PwmStatus");    \
    sdk::ServoController_PwmStatus = reinterpret_cast<jclass>(env->NewGlobalRef(localServoController_PwmStatus));            \
    env->DeleteLocalRef(localServoController_PwmStatus);                                                                     \
    jobject localVoltageUnit = env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/VoltageUnit");            \
    sdk::voltage_unit::VoltageUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localVoltageUnit));                          \
    env->DeleteLocalRef(localVoltageUnit);                                                                                   \
    jobject localVisionPortal = env->FindClass("org/firstinspires/ftc/vision/VisionPortal");                                 \
    sdk::VisionPortal = reinterpret_cast<jclass>(env->NewGlobalRef(localVisionPortal));                                      \
    env->DeleteLocalRef(localVisionPortal);                                                                                  \
    jobject localVisionPortal_Builder = env->FindClass("org/firstinspires/ftc/vision/VisionPortal$Builder");                 \
    sdk::VisionPortal_Builder = reinterpret_cast<jclass>(env->NewGlobalRef(localVisionPortal_Builder));                      \
    env->DeleteLocalRef(localVisionPortal_Builder);                                                                          \
    jobject localColorSensor = env->FindClass("com/qualcomm/robotcore/hardware/ColorSensor");                                \
    sdk::ColorSensor = reinterpret_cast<jclass>(env->NewGlobalRef(localColorSensor));                                        \
    env->DeleteLocalRef(localColorSensor);                                                                                   \
    jobject localTouchSensor = env->FindClass("com/qualcomm/robotcore/hardware/TouchSensor");                                \
    sdk::TouchSensor = reinterpret_cast<jclass>(env->NewGlobalRef(localTouchSensor));                                        \
    env->DeleteLocalRef(localTouchSensor);                                                                                   \
    jobject localAnalogInput = env->FindClass("com/qualcomm/robotcore/hardware/AnalogInput");                                \
    sdk::AnalogInput = reinterpret_cast<jclass>(env->NewGlobalRef(localAnalogInput));                                        \
    env->DeleteLocalRef(localAnalogInput);                                                                                   \
    jobject localDigitalChannel = env->FindClass("com/qualcomm/robotcore/hardware/DigitalChannel");                          \
    sdk::DigitalChannel = reinterpret_cast<jclass>(env->NewGlobalRef(localDigitalChannel));                                  \
    env->DeleteLocalRef(localDigitalChannel);                                                                                \
    jobject localWebcamName = env->FindClass("org/firstinspires/ftc/robotcore/external/hardware/camera/WebcamName");         \
    sdk::WebcamName = reinterpret_cast<jclass>(env->NewGlobalRef(localWebcamName));                                          \
    env->DeleteLocalRef(localWebcamName);                                                                                    \
    jobject localCameraName = env->FindClass("org/firstinspires/ftc/robotcore/external/hardware/camera/CameraName");         \
    sdk::CameraName = reinterpret_cast<jclass>(env->NewGlobalRef(localCameraName));                                          \
    env->DeleteLocalRef(localCameraName);                                                                                    \
    jobject localFirstVisionProcessor = env->FindClass("org/firstinspires/ftc/teamcode/VisionProcessor/FirstVisionProcessor");\
    vision_processor::FirstVisionProcessor = reinterpret_cast<jclass>(env->NewGlobalRef(localFirstVisionProcessor));         \
    env->DeleteLocalRef(localFirstVisionProcessor);                                                                          \
    jobject localSecondVisionProcessor = env->FindClass("org/firstinspires/ftc/teamcode/VisionProcessor/SecondVisionProcessor");\
    vision_processor::SecondVisionProcessor = reinterpret_cast<jclass>(env->NewGlobalRef(localSecondVisionProcessor));       \
    env->DeleteLocalRef(localSecondVisionProcessor);                                                                         \
    sdk::linear_op_mode::init_cpp();                                                                                                                            \

#endif //FTCROBOTCONTROLLER_CPPENV_H