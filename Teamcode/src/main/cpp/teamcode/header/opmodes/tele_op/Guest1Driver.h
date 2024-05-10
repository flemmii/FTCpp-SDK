//
// Created by Frosch PC - V2 on 01.02.2024.
//

#ifndef FTCROBOTCONTROLLER_GUEST_1_DRIVER_H
#define FTCROBOTCONTROLLER_GUEST_1_DRIVER_H


#include "../include_all_header.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "Infonachmittag 1 Driver", group = "Guest")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_Guest1Driver_opMode(JNIEnv *env,
                                                                                 jobject thiz);


#endif //FTCROBOTCONTROLLER_GUEST_1_DRIVER_H
