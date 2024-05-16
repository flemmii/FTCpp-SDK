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

    Mat Vision_processor::process_frame(const Mat &input, long capture_time_nanos) const {
        return input;
    }

    bool operator==(const Vision_processor &lhs, const Vision_processor &rhs) {
        return lhs.id == rhs.id;
    }
}

namespace vision_processor {
    jclass FirstVisionProcessor;
    jclass SecondVisionProcessor;

    bool dual_cam_view = false;

    Mat first_processor_output;
    Mat second_processor_output;

    namespace first_vision_processor {
        const vector <pair<Vision_processor *, bool>> *processors;

        int width = 0;
        int height = 0;

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_FirstVisionProcessor_nativeInit(
                JNIEnv *env,
                jobject thiz,
                jint width,
                jint height,
                jobject calibration) {
            first_vision_processor::width = static_cast<int>(width);
            first_vision_processor::height = static_cast<int>(height);
            for_each(processors->begin(), processors->end(), [&](const auto &processor) {
                if (processor.second)
                    processor.first->init(first_vision_processor::width, first_vision_processor::height);
            });
        }

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_FirstVisionProcessor_nativeProcessFrame(
                JNIEnv *env,
                jobject thiz,
                jlong frame,
                jlong capture_time_nanos) {
            Mat input = *(Mat *) frame;
            Mat output = input.clone();
            for_each(processors->begin(), processors->end(),
                     [input, capture_time_nanos, output](const auto &processor) {
                         if (processor.second) {
                             Mat edited_frame = processor.first->process_frame(input, capture_time_nanos);

                             if (edited_frame.size() != output.size())
                                 resize(edited_frame, edited_frame, output.size()); // Resize result to the same size as output
                             if (edited_frame.type() != output.type())
                                 edited_frame.convertTo(edited_frame, output.type()); // Convert result to the same type as output

                             // TODO: Maybe change this
                             Mat diff;
                             absdiff(input, edited_frame, diff);

                             Mat mask;
                             threshold(diff, mask, 1, 255, THRESH_BINARY);

                             Mat temp;
                             addWeighted(output, 0.3, diff, 0.7, 0, temp);

                             temp.copyTo(output, mask);
                         }
                     });

            output.copyTo(first_processor_output);
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
            // Convert the output Mat to a Bitmap
            vector<uchar> buf;

            Mat picture;

            Mat first_final = first_processor_output.clone();
            Mat second_final = second_processor_output.clone();

            if (dual_cam_view && !second_final.empty()) {

                // Ensure both images have the same number of columns
                if (first_final.cols != second_final.cols) {
                    if (first_final.cols > second_final.cols) {
                        double scale = static_cast<double>(first_final.cols) / second_final.cols;
                        resize(second_final, second_final, Size(), scale, scale, INTER_LINEAR);
                    } else {
                        double scale = static_cast<double>(second_final.cols) / first_final.cols;
                        resize(first_final, first_final, Size(), scale, scale, INTER_LINEAR);
                    }
                }
                vconcat(first_final, second_final, picture);
            } else {
                first_final.copyTo(picture);
            }

            cvtColor(picture, picture, COLOR_BGR2RGB); // OpenCV uses BGR instead of RGB
            imencode(".bmp", picture, buf);
            jbyteArray byteArray = env->NewByteArray(static_cast<int>(buf.size()));
            env->SetByteArrayRegion(byteArray, 0, static_cast<int>(buf.size()),
                                    reinterpret_cast<jbyte *>(buf.data()));

            // Find the BitmapFactory class
            jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");

            // Find the BitmapFactory.decodeByteArray method
            jmethodID decodeByteArrayMethod = env->GetStaticMethodID(bitmapFactoryClass,
                                                                     "decodeByteArray",
                                                                     "([BII)Landroid/graphics/Bitmap;");

            // Call BitmapFactory.decodeByteArray
            jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass,
                                                         decodeByteArrayMethod,
                                                         byteArray, 0,
                                                         static_cast<int>(buf.size()));

            // Find the Bitmap.createScaledBitmap method
            jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
            jmethodID createScaledBitmapMethod = env->GetStaticMethodID(bitmapClass,
                                                                        "createScaledBitmap",
                                                                        "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;");

            jobject scaledBitmap = env->CallStaticObjectMethod(bitmapClass,
                                                               createScaledBitmapMethod,
                                                               bitmap,
                                                               (jint) (width *
                                                                       scale_bmp_px_to_canvas_px),
                                                               (jint) (height *
                                                                       scale_bmp_px_to_canvas_px),
                                                               true);

            // Find the Canvas class and the drawBitmap method
            jmethodID drawBitmapMethod = env->GetMethodID(env->GetObjectClass(canvas),
                                                          "drawBitmap",
                                                          "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V");

            // Draw the scaled Bitmap on the Canvas
            env->CallVoidMethod(canvas, drawBitmapMethod, scaledBitmap, 0.0f, 0.0f,
                                static_cast<jobject>(nullptr));

            env->DeleteLocalRef(byteArray);
            env->DeleteLocalRef(bitmap);
            env->DeleteLocalRef(scaledBitmap);
            env->DeleteLocalRef(bitmapFactoryClass);
            env->DeleteLocalRef(bitmapClass);
        }
    }


    namespace second_vision_processor {
        const vector <pair<Vision_processor *, bool>> *processors;

        int width = 0;
        int height = 0;

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_SecondVisionProcessor_nativeInit(
                JNIEnv *env,
                jobject thiz,
                jint width,
                jint height,
                jobject calibration) {
            second_vision_processor::width = static_cast<int>(width);
            second_vision_processor::height = static_cast<int>(height);

            for_each(processors->begin(), processors->end(), [&](const auto &processor) {
                if (processor.second)
                    processor.first->init(second_vision_processor::width, second_vision_processor::height);
            });
        }

        extern "C"
        JNIEXPORT void JNICALL
        Java_org_firstinspires_ftc_teamcode_VisionProcessor_SecondVisionProcessor_nativeProcessFrame(
                JNIEnv *env,
                jobject thiz,
                jlong frame,
                jlong capture_time_nanos) {
            Mat input = *(Mat *) frame;
            Mat output = input.clone();

            for_each(processors->begin(), processors->end(), [&](const auto &processor) {
                if (processor.second) {
                    Mat edited_frame = processor.first->process_frame(input, capture_time_nanos);

                    if (edited_frame.size() != output.size())
                        resize(edited_frame, edited_frame, output.size()); // Resize result to the same size as output
                    if (edited_frame.type() != output.type())
                        edited_frame.convertTo(edited_frame, output.type()); // Convert result to the same type as output

                    // TODO: Maybe change this
                    Mat diff;
                    absdiff(input, edited_frame, diff);

                    Mat mask;
                    threshold(diff, mask, 1, 255, THRESH_BINARY);

                    Mat temp;
                    addWeighted(output, 0.3, diff, 0.7, 0, temp);

                    temp.copyTo(output, mask);
                }
            });

            output.copyTo(second_processor_output);
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
            // Convert the output Mat to a Bitmap
            vector<uchar> buf;

            Mat picture;

            Mat first_final = first_processor_output.clone();
            Mat second_final = second_processor_output.clone();

            if (dual_cam_view && !first_final.empty()) {
                // Ensure both images have the same number of columns
                if (first_final.cols != second_final.cols) {
                    if (first_final.cols > second_final.cols) {
                        double scale = static_cast<double>(first_final.cols) / second_final.cols;
                        resize(second_final, second_final, Size(), scale, scale, INTER_LINEAR);
                    } else {
                        double scale = static_cast<double>(second_final.cols) / first_final.cols;
                        resize(first_final, first_final, Size(), scale, scale, INTER_LINEAR);
                    }
                }
                vconcat(first_final, second_final, picture);
            } else {
                second_final.copyTo(picture);
            }

            cvtColor(picture, picture, COLOR_BGR2RGB); // OpenCV uses BGR instead of RGB
            imencode(".bmp", picture, buf);
            jbyteArray byteArray = env->NewByteArray(static_cast<int>(buf.size()));
            env->SetByteArrayRegion(byteArray, 0, static_cast<int>(buf.size()),
                                    reinterpret_cast<jbyte *>(buf.data()));

            // Find the BitmapFactory class
            jclass bitmapFactoryClass = env->FindClass("android/graphics/BitmapFactory");

            // Find the BitmapFactory.decodeByteArray method
            jmethodID decodeByteArrayMethod = env->GetStaticMethodID(bitmapFactoryClass,
                                                                     "decodeByteArray",
                                                                     "([BII)Landroid/graphics/Bitmap;");

            // Call BitmapFactory.decodeByteArray
            jobject bitmap = env->CallStaticObjectMethod(bitmapFactoryClass,
                                                         decodeByteArrayMethod,
                                                         byteArray, 0,
                                                         static_cast<int>(buf.size()));

            // Find the Bitmap.createScaledBitmap method
            jclass bitmapClass = env->FindClass("android/graphics/Bitmap");
            jmethodID createScaledBitmapMethod = env->GetStaticMethodID(bitmapClass,
                                                                        "createScaledBitmap",
                                                                        "(Landroid/graphics/Bitmap;IIZ)Landroid/graphics/Bitmap;");

            jobject scaledBitmap = env->CallStaticObjectMethod(bitmapClass,
                                                               createScaledBitmapMethod,
                                                               bitmap,
                                                               (jint) (width *
                                                                       scale_bmp_px_to_canvas_px),
                                                               (jint) (height *
                                                                       scale_bmp_px_to_canvas_px),
                                                               true);

            // Find the Canvas class and the drawBitmap method
            jmethodID drawBitmapMethod = env->GetMethodID(env->GetObjectClass(canvas),
                                                          "drawBitmap",
                                                          "(Landroid/graphics/Bitmap;FFLandroid/graphics/Paint;)V");

            // Draw the scaled Bitmap on the Canvas
            env->CallVoidMethod(canvas, drawBitmapMethod, scaledBitmap, 0.0f, 0.0f,
                                static_cast<jobject>(nullptr));

            env->DeleteLocalRef(byteArray);
            env->DeleteLocalRef(bitmap);
            env->DeleteLocalRef(scaledBitmap);
            env->DeleteLocalRef(bitmapFactoryClass);
            env->DeleteLocalRef(bitmapClass);
        }
    }
}