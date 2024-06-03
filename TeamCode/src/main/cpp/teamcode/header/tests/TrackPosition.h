//
// Created by fnlg on 03.06.2024.
//

#ifndef FTCROBOTCONTROLLER_TEST_H
#define FTCROBOTCONTROLLER_TEST_H

#include <numeric>

#include "sdk.h"

#define ENCODER_TICKS_PER_ROTATION 8192
#define WHEEL_DIAMETER_MM 60

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "trackPosition", group = "tests")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_tests_cpp_TrackPosition_opMode(JNIEnv *env,
                                                                   jobject thiz);


#endif //FTCROBOTCONTROLLER_TEST_H
