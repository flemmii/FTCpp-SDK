//
// Created by fnlg on 09.05.2024.
//

#include "vision/Vision_processor.h"

using namespace std;
using namespace sdk;
using namespace cv;

namespace sdk {
    int Vision_processor::last_id = 0;

    Vision_processor::Vision_processor() {
        id = last_id++;
    }

    void Vision_processor::init(int width, int height) const {}

    void Vision_processor::process_frame(const Mat &input, long capture_time_nanos) const {}

    void Vision_processor::on_draw_frame(int onscreen_width, int onscreen_height,
                                         Mat frame_to_draw_on) const {}

    bool operator==(const Vision_processor &lhs, const Vision_processor &rhs) {
        return lhs.id == rhs.id;
    }
}

namespace vision_processor {
    jclass FirstVisionProcessor;
    jclass SecondVisionProcessor;

    bool dual_cam_view = false;

    Mat first_processor_frame;
    Mat second_processor_frame;

    void
    native_init(int width, int height, const vector<pair<Vision_processor *, bool>> *processors) {
        for_each(processors->begin(), processors->end(),
                 [&](const auto &processor) {
                     if (processor.second)
                         processor.first->init(width, height);
                 });
    }

    void native_process_frame(const Mat &input, long capture_time_nanos,
                              const vector<pair<Vision_processor *, bool>> *processors) {
        Mat output = input.clone();
        for_each(processors->begin(), processors->end(),
                 [&](const auto &processor) {
                     if (processor.second)
                         processor.first->process_frame(input, capture_time_nanos);
                 });
    }

    void native_on_draw_frame(jobject canvas, int onscreen_width, int onscreen_height,
                              float scale_bmp_px_to_canvas_px, float scale_canvas_density,
                              const Mat &frame_to_draw_on,
                              const vector<pair<Vision_processor *, bool>> *processors) {
        Mat output;

        if (dual_cam_view) {
            Mat first_frame = first_processor_frame.clone();
            for_each(first_vision_processor::processors->begin(),
                     first_vision_processor::processors->end(),
                     [&](const auto &processor) {
                         if (processor.second)
                             processor.first->on_draw_frame(onscreen_width, onscreen_height,
                                                            first_frame);
                     });

            Mat second_frame = second_processor_frame.clone();
            for_each(second_vision_processor::processors->begin(),
                     second_vision_processor::processors->end(),
                     [&](const auto &processor) {
                         if (processor.second)
                             processor.first->on_draw_frame(onscreen_width, onscreen_height,
                                                            second_frame);
                     });

            // Ensure both images have the same number of columns
            if (first_frame.cols != second_frame.cols) {
                if (first_frame.cols > second_frame.cols) {
                    double scale = static_cast<double>(first_frame.cols) / second_frame.cols;
                    resize(second_frame, second_frame, Size(), scale, scale, INTER_LINEAR);
                } else {
                    double scale = static_cast<double>(second_frame.cols) / first_frame.cols;
                    resize(first_frame, first_frame, Size(), scale, scale, INTER_LINEAR);
                }
            }
            vconcat(first_frame, second_frame, output);
            resize(output, output, {output.cols, output.rows / 2});
        } else {
            Mat frame = frame_to_draw_on.clone();
            for_each(processors->begin(), processors->end(),
                     [&](const auto &processor) {
                         if (processor.second)
                             processor.first->on_draw_frame(onscreen_width, onscreen_height, frame);
                     });

            frame.copyTo(output);
        }

        // Convert the output Mat to a Bitmap
        attach_thread

        vector<uchar> buf;

        cvtColor(output, output, COLOR_BGR2RGB); // OpenCV uses BGR instead of RGB
        imencode(".bmp", output, buf);
        jbyteArray byteArray = env->NewByteArray(static_cast<int>(buf.size()));
        env->SetByteArrayRegion(byteArray, 0, static_cast<int>(buf.size()),
                                reinterpret_cast<jbyte *>(buf.data()));

        // Find the BitmapFactory class
        jclass BitmapFactory = env->FindClass("android/graphics/BitmapFactory");

        // Call BitmapFactory.decodeByteArray
        jobject bitmap = env->CallStaticObjectMethod(BitmapFactory,
                                                     env->GetStaticMethodID(BitmapFactory,
                                                                            "decodeByteArray",
                                                                            "([BII)Landroid/graphics/Bitmap;"),
                                                     byteArray, 0, static_cast<int>(buf.size()));

        // Find the Bitmap class
        jclass Bitmap = env->FindClass("android/graphics/Bitmap");

        // Call Bitmap.createScaledBitmap
        jobject scaledBitmap = env->CallStaticObjectMethod(Bitmap, env->GetStaticMethodID(Bitmap,
                                                                                          "createScaledBitmap",
                                                                                          "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;"),
                                                           bitmap,
                                                           (jint) (output.cols * scale_bmp_px_to_canvas_px),
                                                           (jint) (output.rows * scale_bmp_px_to_canvas_px),
                                                           true);

        // Find the Canvas class
        jclass Canvas = env->GetObjectClass(canvas);

        // Draw the scaled Bitmap on the Canvas
        env->CallVoidMethod(canvas, env->GetMethodID(Canvas, "drawBitmap",
                                                     "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V"),
                            scaledBitmap, 0.0f, 0.0f, static_cast<jobject>(nullptr));

        // Delete the local references
        env->DeleteLocalRef(byteArray);
        env->DeleteLocalRef(bitmap);
        env->DeleteLocalRef(scaledBitmap);
        env->DeleteLocalRef(BitmapFactory);
        env->DeleteLocalRef(Bitmap);
        env->DeleteLocalRef(Canvas);
    }

    namespace first_vision_processor {
        const vector<pair<Vision_processor *, bool>> *processors;

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_FirstVisionProcessor_nativeInit(
                JNIEnv *env,
                jobject thiz,
                jint width,
                jint height,
                jobject calibration) {
            native_init(static_cast<int>(width), static_cast<int>(height), processors);
        }

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_FirstVisionProcessor_nativeProcessFrame(
                JNIEnv *env,
                jobject thiz,
                jlong frame,
                jlong capture_time_nanos) {
            first_processor_frame = *(Mat *) frame;
            native_process_frame(*(Mat *) frame, static_cast<long>(capture_time_nanos), processors);
        }

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_FirstVisionProcessor_nativeOnDrawFrame(
                JNIEnv *env,
                jobject thiz,
                jobject canvas,
                jint onscreen_width,
                jint onscreen_height,
                jfloat scale_bmp_px_to_canvas_px,
                jfloat scale_canvas_density,
                jobject user_context) {
            native_on_draw_frame(canvas, static_cast<int>(onscreen_width),
                                 static_cast<int>(onscreen_height),
                                 static_cast<float>(scale_bmp_px_to_canvas_px),
                                 static_cast<float>(scale_canvas_density), first_processor_frame,
                                 processors);
        }
    }


    namespace second_vision_processor {
        const vector<pair<Vision_processor *, bool>> *processors;

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_SecondVisionProcessor_nativeInit(
                JNIEnv *env,
                jobject thiz,
                jint width,
                jint height,
                jobject calibration) {
            native_init(static_cast<int>(width), static_cast<int>(height), processors);
        }

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_SecondVisionProcessor_nativeProcessFrame(
                JNIEnv *env,
                jobject thiz,
                jlong frame,
                jlong capture_time_nanos) {
            second_processor_frame = *(Mat *) frame;
            native_process_frame(*(Mat *) frame, static_cast<long>(capture_time_nanos), processors);
        }

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_SecondVisionProcessor_nativeOnDrawFrame(
                JNIEnv *env,
                jobject thiz,
                jobject canvas,
                jint onscreen_width,
                jint onscreen_height,
                jfloat scale_bmp_px_to_canvas_px,
                jfloat scale_canvas_density,
                jobject user_context) {
            native_on_draw_frame(canvas, static_cast<int>(onscreen_width),
                                 static_cast<int>(onscreen_height),
                                 static_cast<float>(scale_bmp_px_to_canvas_px),
                                 static_cast<float>(scale_canvas_density), second_processor_frame,
                                 processors);
        }
    }
}