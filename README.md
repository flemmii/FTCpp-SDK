# Downloading this project

To download this repository, simply run the following command in your terminal:

```bash
git clone https://github.com/flemmii/FTCpp-SDK.git
```

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

If it says anything with JNI and you have done nothing with JNI, its probably an error in the sdk.
If that happens please open an issue on GitHub or write me on Discord (flemmii).
Please send me what you tried to do, so the code you used and the error message.

# Contributing

If you want to contribute to this project, you can simply open a pull request on GitHub.
It doesn't matter if you want to add a new feature, fix a bug, add an example or just improve
the documentation.

If you want to help me even further multiple times consider to DM me on Discord (flemmii), so
I can add you to the project and you don't need to open pull request every time.

## Feature requests

If you have any feature requests, you can open an issue on GitHub or write me on Discord (flemmii).
I will try to implement them as soon as possible. You can also try to implement them yourself, if
you do so please open a pull request on GitHub to share your code with others.