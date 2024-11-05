package org.firstinspires.ftc.teamcode.sdk;

import android.content.Context;

import com.qualcomm.robotcore.eventloop.opmode.AnnotatedOpModeManager;
import com.qualcomm.robotcore.eventloop.opmode.OpModeRegistrar;


public class LibLoader {
    private static final boolean alreadyLoaded = false;

    /*
     * By annotating this method with @OpModeRegistrar, it will be called
     * automatically by the SDK as it is scanning all the classes in the app
     * (for @Teleop, etc.) while it is "starting" the robot.
     */
    @OpModeRegistrar
    public static void loadNativeLibOnStartRobot(Context context, AnnotatedOpModeManager manager) {
        /*
         * Because this is called every time the robot is "restarted" we
         * check to see whether we've already previously done our job here.
         */
        if (alreadyLoaded) {
            /*
             * Get out of dodge
             */
            return;
        }

        System.loadLibrary("sdk");
        System.loadLibrary("teamcode");
    }
}
