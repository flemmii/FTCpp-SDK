//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_MAIN_H
#define FTCROBOTCONTROLLER_MAIN_H

#include "../include_all_header.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "Main")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_CppTest_opMode(JNIEnv *env,
                                                                            jobject thiz);

#endif //FTCROBOTCONTROLLER_MAIN_H