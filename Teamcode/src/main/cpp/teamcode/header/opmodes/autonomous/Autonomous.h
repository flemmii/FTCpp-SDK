//
// Created by Frosch PC - V2 on 07.02.2024.
//

#ifndef FTCROBOTCONTROLLER_AUTONOMOUS_H
#define FTCROBOTCONTROLLER_AUTONOMOUS_H

#include "../include_all_header.h"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @Autonomous(name = "Autonomous", preselectTeleOp = "Main")
 */
extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_autonomous_AutonomousAllmaechtig_opMode(JNIEnv *env,
                                                                                    jobject thiz);


#endif //FTCROBOTCONTROLLER_AUTONOMOUS_H
