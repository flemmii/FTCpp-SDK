//
// Created by Frosch PC - V2 on 04.06.2024.
//

#ifndef FTCROBOTCONTROLLER_DRIVING_H
#define FTCROBOTCONTROLLER_DRIVING_H

//
// Created by fnlg on 12.05.2024.
//

#ifndef FTCROBOTCONTROLLER_EXAMPLE_H
#define FTCROBOTCONTROLLER_EXAMPLE_H

#include <vector>
#include <chrono>
#include <numeric>

#include "sdk.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "DrivingCpp", group = "tests")
 */

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_tests_cpp_DrivingCpp_opMode(JNIEnv *env,
                                                                  jobject thiz);

#endif //FTCROBOTCONTROLLER_EXAMPLE_H


#endif //FTCROBOTCONTROLLER_DRIVING_H