# Downloading this project

To download this repository, simply run the following command in your terminal:

```bash
git clone https://github.com/flemmii/FTCpp-SDK.git
```

After cloning you need to execute the build process once so after that everything is set up
and you can download the code normally to your robot. Also after a clean you need to
execute the build process once before downloading. (This will get changed later)

Also make sure that you have node installed, otherwise [download it here](https://nodejs.org/en/download/prebuilt-installer)

Remember you need to merge this repository into your own repository to get the latest
updates.

## Excluding the example from downloading

If you don't want the example to be downloaded to your repository ever time you pull you can
you can just run this command in the root of your project:

```bash
git update-index --assume-unchanged TeamCode/src/main/cpp/teamcode/header/example.h
git update-index --assume-unchanged TeamCode/src/main/cpp/teamcode/sources/example.cpp
```

# The different folders

## createOpModeJava

This folder contains two template files that are used to create the equivalent OpMode in Java.
It also has the JavaScript file that creates the files in Java.

You do not need to do anything in this folder.

## external_libraries

This folder contains the external libraries that are used in the project:

- [OpenCV](https://github.com/opencv/opencv)
- [Apriltag](https://github.com/AprilRobotics/apriltag)

The sources of these projects are already included by the FTC SDK.

If you want to add any of other libraries put the header files in there.

## sdk

This folder contains the C++ framework of the SDK that is used to communicate with the
robot controller.

You can add your own sensors, motors or whatever you need. If you do so it'd be nice if you
could open a pull request on GitHub to share your code with others.

## teamcode

This module, teamcode, is the place where you will write/paste the code for your team's
robot controller App. This module has currently only one example but the
process for adding OpModes is straightforward.

## Creating your own OpModes

The easiest way to create your own OpMode is to copy a Example OpMode and make it your own.

But if you want to create a new OpMode from scratch you can use the following steps:

- Create a new file in the header folder with the name of your OpMode
- Create a new file in the source folder with the name of your OpMode
- Add the following code to the header file:

```cpp
#include <jni.h>
#include "sdk.h"

// Your OpMode annotation shown below
    
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_teleop_Example_opMode(JNIEnv *env,
                                                              jobject thiz);
```

- Add the following code to the source file:

```cpp
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_teleop_Example_opMode(JNIEnv *env,
                                                              jobject thiz) {
    init_sdk
}
```

- Change the path to your OpMode by just changing the
  Java_org_firstinspires_ftc_teamcode_opmodes_teleop_Example_opMode to your path.
    - The penultimate part of the path is the name of the Java file and Class
    - The last part is the name of the method in there

### Annotations

You got the same annotations as in Java, the only difference is that they need to be in a
comment in you header file of your OpMode in the header folder.

```
 @TeleOp(name="Example", group="Example")
 @Disabled
```

The name that will appear on the driver station's "opmode list" is defined by the code:
``name="Template: Linear OpMode"``
You can change what appears between the quotes to better describe your opmode.
The "group=" portion of the code can be used to help organize your list of OpModes.

As shown, the current OpMode will NOT appear on the driver station's OpMode list because of the
``@Disabled`` annotation which has been included.
This line can simply be deleted to make the OpMode visible.

### Changing the folder structure

If you want to move your header to another folder simply head to the createOpModeJava/index.js
file and change the folder name in line 36 to the path of your desire.
Also change the path in the CMakelists.txt in line 70 to the same path.

If you want to move your sources to another folder you just need to change the path in the
CMakelists.txt in line 18 to the path of your desire.

# Troubleshooting

You can always open an issue on GitHub or write me on Discord (flemmii) if you have any problems.
But please try to solve the problem yourself first.

## Build fails

You get normal error messages in the terminal, so you can see what went wrong.

## Code crashes on robot

You can only see the errors in logcat, so nothing will be shown on the Driver Hub.
If you want to log anything additional to logcat simply include the sdk/extras/utils.h file
and use the log function.

<details close>
  <summary> How a log looks like </summary>
  2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470] JNI DETECTED ERROR IN APPLICATION: JNI NewGlobalRef called with pending exception java.lang.IllegalArgumentException: Unable to find a hardware device with name "test" and type DcMotorEx
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at java.lang.Object com.qualcomm.robotcore.hardware.HardwareMap.get(java.lang.Class, java.lang.String) (HardwareMap.java:213)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void org.firstinspires.ftc.teamcode.opmodes.autonomous.AutoTest.opMode() (AutoTest.java:-2)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void org.firstinspires.ftc.teamcode.opmodes.autonomous.AutoTest.runOpMode() (AutoTest.java:15)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.eventloop.opmode.LinearOpMode.internalRunOpMode() (LinearOpMode.java:199)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.eventloop.opmode.OpModeInternal.lambda$internalInit$1$com-qualcomm-robotcore-eventloop-opmode-OpModeInternal() (OpModeInternal.java:181)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.eventloop.opmode.OpModeInternal$$ExternalSyntheticLambda1.run() (D8$$SyntheticClass:-1)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.util.ThreadPool.logThreadLifeCycle(java.lang.String, java.lang.Runnable) (ThreadPool.java:737)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.eventloop.opmode.OpModeInternal.lambda$internalInit$2$com-qualcomm-robotcore-eventloop-opmode-OpModeInternal() (OpModeInternal.java:179)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.eventloop.opmode.OpModeInternal$$ExternalSyntheticLambda2.run() (D8$$SyntheticClass:-1)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void java.util.concurrent.ThreadPoolExecutor.runWorker(java.util.concurrent.ThreadPoolExecutor$Worker) (ThreadPoolExecutor.java:1133)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void java.util.concurrent.ThreadPoolExecutor$Worker.run() (ThreadPoolExecutor.java:607)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void com.qualcomm.robotcore.util.ThreadPool$ThreadFactoryImpl$1.run() (ThreadPool.java:793)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   at void java.lang.Thread.run() (Thread.java:761)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470] 
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]     in call to NewGlobalRef
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]     from void org.firstinspires.ftc.teamcode.opmodes.autonomous.AutoTest.opMode()
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470] "OpModeThread" prio=5 tid=58 Runnable
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   | group="main" sCount=0 dsCount=0 obj=0x137fc280 self=0x782599e200
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   | sysTid=3589 nice=0 cgrp=default sched=0/0 handle=0x781fdf0450
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   | state=R schedstat=( 0 0 0 ) utm=106 stm=2 core=2 HZ=100
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   | stack=0x781fcee000-0x781fcf0000 stackSize=1037KB
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   | held mutexes= "mutator lock"(shared held)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #00 pc 000000000047ef3c  /system/lib64/libart.so (_ZN3art15DumpNativeStackERNSt3__113basic_ostreamIcNS0_11char_traitsIcEEEEiP12BacktraceMapPKcPNS_9ArtMethodEPv+220)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #01 pc 000000000047ef38  /system/lib64/libart.so (_ZN3art15DumpNativeStackERNSt3__113basic_ostreamIcNS0_11char_traitsIcEEEEiP12BacktraceMapPKcPNS_9ArtMethodEPv+216)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #02 pc 0000000000452fc4  /system/lib64/libart.so (_ZNK3art6Thread9DumpStackERNSt3__113basic_ostreamIcNS1_11char_traitsIcEEEEbP12BacktraceMap+480)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #03 pc 00000000002f04a4  /system/lib64/libart.so (_ZN3art9JavaVMExt8JniAbortEPKcS2_+1136)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #04 pc 00000000002f0bb4  /system/lib64/libart.so (_ZN3art9JavaVMExt9JniAbortVEPKcS2_St9__va_list+124)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #05 pc 0000000000102798  /system/lib64/libart.so (_ZN3art11ScopedCheck6AbortFEPKcz+156)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #06 pc 00000000001021a8  /system/lib64/libart.so (_ZN3art11ScopedCheck11CheckThreadEP7_JNIEnv+536)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #07 pc 00000000000ffce8  /system/lib64/libart.so (_ZN3art11ScopedCheck5CheckERNS_18ScopedObjectAccessEbPKcPNS_12JniValueTypeE+1124)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #08 pc 0000000000103a7c  /system/lib64/libart.so (_ZN3art8CheckJNI6NewRefEPKcP7_JNIEnvP8_jobjectNS_15IndirectRefKindE+624)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #09 pc 0000000000014648  /data/app/com.qualcomm.ftcrobotcontroller-1/lib/arm64/libsdk.so (_ZN7_JNIEnv12NewGlobalRefEP8_jobject+40)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #10 pc 000000000001c7f0  /data/app/com.qualcomm.ftcrobotcontroller-1/lib/arm64/libsdk.so (_ZN3sdk12hardware_map3getEP7_jclassRKNSt6__ndk112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE+236)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #11 pc 0000000000070f04  /data/app/com.qualcomm.ftcrobotcontroller-1/lib/arm64/libteamcode.so (Java_org_firstinspires_ftc_teamcode_opmodes_autonomous_AutoTest_opMode+3036)
2024-06-05 14:16:51.067  3446-3589  art                     com.qualcomm.ftcrobotcontroller      A  art/runtime/java_vm_ext.cc:470]   native: #12 pc 000000000011e230  /data/app/com.qualcomm.ftcrobotcontroller-1/oat/arm64/base.odex (Java_org_firstinspires_ftc_teamcode_opmodes_autonomous_AutoTest_opMode__+124)
</summary>

If it says anything with JNI and you have done nothing with JNI, its probably an error in the sdk.
If that happens please open an issue on GitHub or write me on Discord (flemmii).
Please send me what you tried to do, so the code you used and the error message.

# Contributing

If you want to contribute to this project, you can simply open a pull request on GitHub.
It doesn't matter if you want to add a new feature, fix a bug, add an example or just improve
the documentation.

If you want to help me even further consider to DM me on Discord (flemmii), so
I can add you to the project and you don't need to open pull request every time.

## Feature requests

If you have any feature requests, you can open an issue on GitHub or write me on Discord (flemmii).
I will try to implement them as soon as possible. You can also try to implement them yourself, if
you do so please open a pull request on GitHub to share your code with others.
