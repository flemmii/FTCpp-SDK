//
// Created by fnlg on 12.05.2024.
//

#ifndef FTCROBOTCONTROLLER_EXAMPLE_H
#define FTCROBOTCONTROLLER_EXAMPLE_H

#include <jni.h>
#include "sdk.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "Example", group = "Example")
 */

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_teleop_Example_opMode(JNIEnv *env,
                                                                  jobject thiz);

#endif //FTCROBOTCONTROLLER_EXAMPLE_H
