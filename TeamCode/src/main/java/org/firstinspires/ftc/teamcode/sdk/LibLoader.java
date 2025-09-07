package org.firstinspires.ftc.teamcode.sdk;

public class LibLoader {
    static {
        System.load("//data/local/tmp/liblib_loader.so");
    }

    public static native void run(Object object);
}