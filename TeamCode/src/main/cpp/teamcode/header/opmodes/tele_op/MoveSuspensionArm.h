//
// Created by Frosch PC - V2 on 05.02.2024.
//

#ifndef FTCROBOTCONTROLLER_MOVESUSPENSIONARM_H
#define FTCROBOTCONTROLLER_MOVESUSPENSIONARM_H

#include "../include_all_header.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "Move Suspension Arm")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_MoveSuspensionArm_opMode(JNIEnv *env,
                                                                                      jobject thiz);

#endif //FTCROBOTCONTROLLER_MOVESUSPENSIONARM_H
