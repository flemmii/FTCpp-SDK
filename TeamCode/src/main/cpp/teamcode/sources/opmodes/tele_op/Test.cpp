//
// Created by fnlg on 05.04.2024.
//

#include "../../../header/opmodes/tele_op/Test.h"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace gamepads;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_remotecontrolled_Test_opMode(JNIEnv *env,
                                                                         jobject thiz) {
    init_sdk
    robot::init();

    wait_for_start();

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        logcat_log(ANDROID_LOG_ERROR, "Couldn't open the camera");
    }

    cv::Mat frame;
    // Erfassen Sie Bilder von der Kamera
    while (!linear_op_mode::is_stop_requested()) {
        // Erfassen Sie ein Bild von der Kamera
        cap >> frame;

        // Überprüfen, ob das Bild erfasst wurde
        if (frame.empty()) {
            logcat_log(ANDROID_LOG_ERROR, "Couldn't capture a frame");
            break;
        }
    }

    // Kamera freigeben und Fenster schließen
    cap.release();
}