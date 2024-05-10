//
// Created by fnlg on 05.04.2024.
//

#ifndef FTCROBOTCONTROLLER_TEST_H
#define FTCROBOTCONTROLLER_TEST_H

#include "../include_all_header.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "Test")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_Test_opMode(JNIEnv *env,
                                                                         jobject thiz);


#endif //FTCROBOTCONTROLLER_TEST_H
