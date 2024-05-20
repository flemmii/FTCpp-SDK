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
    jclass localCurrentOpMode = env->GetObjectClass(thiz);                                                                  \
    sdk::CurrentOpMode = reinterpret_cast<jclass>(env->NewGlobalRef(localCurrentOpMode));                                    \
    env->DeleteLocalRef(localCurrentOpMode);                                                                                 \
    jclass localDcMotorEx = env->FindClass("com/qualcomm/robotcore/hardware/DcMotorEx");                                    \
    sdk::DcMotorEx = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotorEx));                                            \
    env->DeleteLocalRef(localDcMotorEx);                                                                                     \
    jclass localDcMotor_RunMode = env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$RunMode");                        \
    sdk::DcMotor_RunMode = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotor_RunMode));                                \
    env->DeleteLocalRef(localDcMotor_RunMode);                                                                               \
    jclass localPIDFCoefficients = env->FindClass("com/qualcomm/robotcore/hardware/PIDFCoefficients");                      \
    sdk::PIDFCoefficients = reinterpret_cast<jclass>(env->NewGlobalRef(localPIDFCoefficients));                              \
    env->DeleteLocalRef(localPIDFCoefficients);                                                                              \
    jclass localDcMotor_ZeroPowerBehavior = env->FindClass("com/qualcomm/robotcore/hardware/DcMotor$ZeroPowerBehavior");    \
    sdk::DcMotor_ZeroPowerBehavior = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotor_ZeroPowerBehavior));            \
    env->DeleteLocalRef(localDcMotor_ZeroPowerBehavior);                                                                     \
    jclass localDcMotorSimple = env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple");                            \
    sdk::DcMotorSimple = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotorSimple));                                    \
    env->DeleteLocalRef(localDcMotorSimple);                                                                                 \
    jclass localDcMotorSimple_Direction = env->FindClass("com/qualcomm/robotcore/hardware/DcMotorSimple$Direction");        \
    sdk::DcMotorSimple_Direction = reinterpret_cast<jclass>(env->NewGlobalRef(localDcMotorSimple_Direction));                \
    env->DeleteLocalRef(localDcMotorSimple_Direction);                                                                       \
    jclass localGamepad = env->FindClass("com/qualcomm/robotcore/hardware/Gamepad");                                        \
    sdk::Gamepad = reinterpret_cast<jclass>(env->NewGlobalRef(localGamepad));                                                \
    env->DeleteLocalRef(localGamepad);                                                                                       \
    jclass localTelemetry = env->FindClass("org/firstinspires/ftc/robotcore/external/Telemetry");                           \
    sdk::Telemetry = reinterpret_cast<jclass>(env->NewGlobalRef(localTelemetry));                                            \
    env->DeleteLocalRef(localTelemetry);                                                                                     \
    jclass localLynxModule = env->FindClass("com/qualcomm/hardware/lynx/LynxModule");                                       \
    sdk::LynxModule = reinterpret_cast<jclass>(env->NewGlobalRef(localLynxModule));                                          \
    env->DeleteLocalRef(localLynxModule);                                                                                    \
    jclass localBulkCachingMode = env->FindClass("com/qualcomm/hardware/lynx/LynxModule$BulkCachingMode");                  \
    sdk::BulkCachingMode = reinterpret_cast<jclass>(env->NewGlobalRef(localBulkCachingMode));                                \
    env->DeleteLocalRef(localBulkCachingMode);                                                                               \
    jclass localBNO055IMU = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU");                                        \
    sdk::BNO055IMU = reinterpret_cast<jclass>(env->NewGlobalRef(localBNO055IMU));                                            \
    env->DeleteLocalRef(localBNO055IMU);                                                                                     \
    jclass localBNO055IMU_Parameters = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$Parameters");                            \
    sdk::BNO055IMU_Parameters = reinterpret_cast<jclass>(env->NewGlobalRef(localBNO055IMU_Parameters));                                          \
    env->DeleteLocalRef(localBNO055IMU_Parameters);                                                                                    \
    jclass localBNO055IMU_AngleUnit = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$AngleUnit");                              \
    sdk::BNO055IMU_AngleUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localBNO055IMU_AngleUnit));                                            \
    env->DeleteLocalRef(localBNO055IMU_AngleUnit);                                                                                     \
    jclass localBNO055IMU_AccelUnit = env->FindClass("com/qualcomm/hardware/bosch/BNO055IMU$AccelUnit");                              \
    sdk::BNO055IMU_AccelUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localBNO055IMU_AccelUnit));                                            \
    env->DeleteLocalRef(localBNO055IMU_AccelUnit);                                                                                     \
    jclass localIMU = env->FindClass("com/qualcomm/hardware/IMU");                                                          \
    sdk::IMU = reinterpret_cast<jclass>(env->NewGlobalRef(localIMU));                                                       \
    env->DeleteLocalRef(localIMU);                                                                                           \
    jclass localDistanceSensor = env->FindClass("com/qualcomm/robotcore/hardware/DistanceSensor");                          \
    sdk::DistanceSensor = reinterpret_cast<jclass>(env->NewGlobalRef(localDistanceSensor));                                  \
    env->DeleteLocalRef(localDistanceSensor);                                                                                \
    jclass localDistanceUnit = env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/DistanceUnit");          \
    sdk::DistanceUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localDistanceUnit));                                      \
    env->DeleteLocalRef(localDistanceUnit);                                                                                  \
    jclass localServo = env->FindClass("com/qualcomm/robotcore/hardware/Servo");                                            \
    sdk::Servo = reinterpret_cast<jclass>(env->NewGlobalRef(localServo));                                                    \
    env->DeleteLocalRef(localServo);                                                                                         \
    jclass localServo_Direction = env->FindClass("com/qualcomm/robotcore/hardware/Servo$Direction");                        \
    sdk::Servo_Direction = reinterpret_cast<jclass>(env->NewGlobalRef(localServo_Direction));                                \
    env->DeleteLocalRef(localServo_Direction);                                                                               \
    jclass localServoController = env->FindClass("com/qualcomm/robotcore/hardware/ServoController");                        \
    sdk::ServoController = reinterpret_cast<jclass>(env->NewGlobalRef(localServoController));                                \
    env->DeleteLocalRef(localServoController);                                                                               \
    jclass localServoController_PwmStatus = env->FindClass("com/qualcomm/robotcore/hardware/ServoController$PwmStatus");    \
    sdk::ServoController_PwmStatus = reinterpret_cast<jclass>(env->NewGlobalRef(localServoController_PwmStatus));            \
    env->DeleteLocalRef(localServoController_PwmStatus);                                                                     \
    jclass localVoltageUnit = env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/VoltageUnit");            \
    sdk::VoltageUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localVoltageUnit));                                       \
    env->DeleteLocalRef(localVoltageUnit);                                                                                   \
    jclass localAngleUnit = env->FindClass("org/firstinspires/ftc/robotcore/external/navigation/AngleUnit");            \
    sdk::AngleUnit = reinterpret_cast<jclass>(env->NewGlobalRef(localAngleUnit));                                       \
    env->DeleteLocalRef(localAngleUnit);                                                                                  \
    jclass localVisionPortal = env->FindClass("org/firstinspires/ftc/vision/VisionPortal");                                 \
    sdk::VisionPortal = reinterpret_cast<jclass>(env->NewGlobalRef(localVisionPortal));                                      \
    env->DeleteLocalRef(localVisionPortal);                                                                                  \
    jclass localVisionPortal_Builder = env->FindClass("org/firstinspires/ftc/vision/VisionPortal$Builder");                 \
    sdk::VisionPortal_Builder = reinterpret_cast<jclass>(env->NewGlobalRef(localVisionPortal_Builder));                      \
    env->DeleteLocalRef(localVisionPortal_Builder);                                                                          \
    jclass localVisionPortal_MultiPortalLayout = env->FindClass("org/firstinspires/ftc/vision/VisionPortal$MultiPortalLayout");\
    sdk::VisionPortal_MultiPortalLayout = reinterpret_cast<jclass>(env->NewGlobalRef(localVisionPortal_MultiPortalLayout));  \
    env->DeleteLocalRef(localVisionPortal_MultiPortalLayout);                                                               \
    jclass localColorSensor = env->FindClass("com/qualcomm/robotcore/hardware/ColorSensor");                                \
    sdk::ColorSensor = reinterpret_cast<jclass>(env->NewGlobalRef(localColorSensor));                                        \
    env->DeleteLocalRef(localColorSensor);                                                                                   \
    jclass localTouchSensor = env->FindClass("com/qualcomm/robotcore/hardware/TouchSensor");                                \
    sdk::TouchSensor = reinterpret_cast<jclass>(env->NewGlobalRef(localTouchSensor));                                        \
    env->DeleteLocalRef(localTouchSensor);                                                                                   \
    jclass localAnalogInput = env->FindClass("com/qualcomm/robotcore/hardware/AnalogInput");                                \
    sdk::AnalogInput = reinterpret_cast<jclass>(env->NewGlobalRef(localAnalogInput));                                        \
    env->DeleteLocalRef(localAnalogInput);                                                                                   \
    jclass localDigitalChannel = env->FindClass("com/qualcomm/robotcore/hardware/DigitalChannel");                          \
    sdk::DigitalChannel = reinterpret_cast<jclass>(env->NewGlobalRef(localDigitalChannel));                                  \
    env->DeleteLocalRef(localDigitalChannel);                                                                                \
    jclass localWebcamName = env->FindClass("org/firstinspires/ftc/robotcore/external/hardware/camera/WebcamName");         \
    sdk::WebcamName = reinterpret_cast<jclass>(env->NewGlobalRef(localWebcamName));                                          \
    env->DeleteLocalRef(localWebcamName);                                                                                    \
    jclass localCameraName = env->FindClass("org/firstinspires/ftc/robotcore/external/hardware/camera/CameraName");         \
    sdk::CameraName = reinterpret_cast<jclass>(env->NewGlobalRef(localCameraName));                                          \
    env->DeleteLocalRef(localCameraName);                                                                                    \
    jclass localFirstVisionProcessor = env->FindClass("org/firstinspires/ftc/teamcode/VisionProcessor/FirstVisionProcessor");\
    vision_processor::FirstVisionProcessor = reinterpret_cast<jclass>(env->NewGlobalRef(localFirstVisionProcessor));         \
    env->DeleteLocalRef(localFirstVisionProcessor);                                                                          \
    jclass localSecondVisionProcessor = env->FindClass("org/firstinspires/ftc/teamcode/VisionProcessor/SecondVisionProcessor");\
    vision_processor::SecondVisionProcessor = reinterpret_cast<jclass>(env->NewGlobalRef(localSecondVisionProcessor));       \
    env->DeleteLocalRef(localSecondVisionProcessor);                                                                         \
    sdk::linear_op_mode::init_cpp();                                                                                                                            \

#endif //FTCROBOTCONTROLLER_CPPENV_H