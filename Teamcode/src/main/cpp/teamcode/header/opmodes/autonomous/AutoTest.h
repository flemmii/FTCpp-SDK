//
// Created by Frosch PC - V2 on 19.03.2024.
//

#ifndef FTCROBOTCONTROLLER_AUTOTEST_H
#define FTCROBOTCONTROLLER_AUTOTEST_H

#include "../include_all_header.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @Autonomous(name = "AutoTest")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_autonomous_AutoTest_opMode(JNIEnv *env,
                                                                       jobject thiz);


#endif //FTCROBOTCONTROLLER_AUTOTEST_H
