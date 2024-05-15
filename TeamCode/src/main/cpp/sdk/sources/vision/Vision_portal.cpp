//
// Created by Frosch PC - V2 on 05.02.2024.
//

#include "vision/Vision_portal.h"

using namespace std;
using namespace sdk;

namespace sdk {
    jclass VisionPortal;
    jclass VisionPortal_MultiPortalLayout;
    jclass VisionPortal_Builder;

    int Vision_portal::vision_portal_count = 0;
    int Vision_portal::DEFAULT_VIEW_CONTAINER_ID;

    const char *
    Vision_portal::multi_portal_layout_to_string(Vision_portal::Multi_portal_layout mpl) {
        switch (mpl) {
            case Multi_portal_layout::HORIZONTAL:
                return "HORIZONTAL";
            case Multi_portal_layout::VERTICAL:
                return "VERTICAL";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    const char *Vision_portal::stream_format_to_string(Vision_portal::Stream_format sf) {
        switch (sf) {
            case Stream_format::YUY2:
                return "YUY2";
            case Stream_format::MJPEG:
                return "MJPEG";
            default:
                throw invalid_argument("Unimplemented item");
        }
    }

    Vision_portal::Vision_portal(jobject visionPortal) : visionPortal(visionPortal) {
        // Normally you just need 2 Vision_portals and more than 2 Vision_portals have also a high risk to crash the control hub
        if (vision_portal_count > 2)
            throw runtime_error("Only 2 Vision_portals are allowed to exist at a time");
        vision_portal_count++;

        if (vision_portal_count == 1)
            vision_processor::first_vision_processor::processors = &processors;
        else
            vision_processor::second_vision_processor::processors = &processors;

        attach_thread

        DEFAULT_VIEW_CONTAINER_ID = env->GetStaticIntField(VisionPortal,
                                                           env->GetStaticFieldID(VisionPortal,
                                                                                 "DEFAULT_VIEW_CONTAINER_ID",
                                                                                 "I"));
    }

    Vision_portal::~Vision_portal() {
        vision_portal_count--;
    }

    vector<int>
    Vision_portal::makeMultiPortalView(int numPortals, Vision_portal::Multi_portal_layout mpl) {
        attach_thread
        jobject jmpl = env->GetStaticObjectField(sdk::VisionPortal_MultiPortalLayout,
                                                 env->GetStaticFieldID(
                                                         VisionPortal_MultiPortalLayout,
                                                         multi_portal_layout_to_string(mpl),
                                                         "Lorg/firstinspires/ftc/vision/VisionPortal$MultiPortalLayout;"));

        auto jarray = (jintArray) (env->CallStaticObjectMethod(VisionPortal,
                                                               env->GetStaticMethodID(VisionPortal,
                                                                                      "makeMultiPortalView",
                                                                                      "(ILorg/firstinspires/ftc/vision/VisionPortal$MultiPortalLayout;)[I"),
                                                               static_cast<jint>(numPortals),
                                                               jmpl));
        jint *elements = env->GetIntArrayElements(jarray, nullptr);
        jsize size = env->GetArrayLength(jarray);

        vector<int> array(size);
        for (int i = 0; i < size; i++)
            array.push_back(elements[i]);

        env->DeleteLocalRef(jmpl);
        env->ReleaseIntArrayElements(jarray, elements, JNI_ABORT);

        return array;
    }

    Vision_portal Vision_portal::easy_create_with_defaults(const Camera_name &camera_name,
                                                           const std::vector<Vision_processor> &processors) {
        return Vision_portal::Builder().set_camera(camera_name).add_processors(processors).build();
    }

    void Vision_portal::enable_dual_cam_view() {
        vision_processor::dual_cam_view = true;
    }

    void Vision_portal::disable_dual_cam_view() {
        vision_processor::dual_cam_view = false;
    }

    void
    Vision_portal::set_processor_enabled(const sdk::Vision_processor &processor,
                                         bool enabled) {
        for (auto &proc: processors) {
            if (proc.first == processor) {
                proc.second = enabled;
                return;
            }
        }
    }

    bool
    Vision_portal::get_processor_enabled(const sdk::Vision_processor &processor) {
        for (const auto &proc: processors) {
            if (proc.first == processor)
                return proc.second;
        }
        return false;
    }

    Vision_portal::Camera_state Vision_portal::get_camera_state() const {
        attach_thread
        jobject javaEnum = env->CallObjectMethod(visionPortal,
                                                 env->GetMethodID(VisionPortal, "getCameraState",
                                                                  "()Lorg/firstinspires/ftc/vision/VisionPortal$CameraState;"));
        jclass enumClass = env->GetObjectClass(javaEnum);
        jmethodID nameMethod = env->GetMethodID(enumClass, "name", "()Ljava/lang/String;");
        auto name = (jstring) env->CallObjectMethod(javaEnum, nameMethod);

        const char *nameCStr = env->GetStringUTFChars(name, nullptr);
        std::string nameStr(nameCStr);
        env->ReleaseStringUTFChars(name, nameCStr);

        if (nameStr == "OPENING_CAMERA_DEVICE")
            return Vision_portal::Camera_state::OPENING_CAMERA_DEVICE;
        else if (nameStr == "CAMERA_DEVICE_READY")
            return Vision_portal::Camera_state::CAMERA_DEVICE_READY;
        else if (nameStr == "STARTING_STREAM")
            return Vision_portal::Camera_state::STARTING_STREAM;
        else if (nameStr == "STREAMING")
            return Vision_portal::Camera_state::STREAMING;
        else if (nameStr == "STOPPING_STREAM")
            return Vision_portal::Camera_state::STOPPING_STREAM;
        else if (nameStr == "CLOSING_CAMERA_DEVICE")
            return Vision_portal::Camera_state::CLOSING_CAMERA_DEVICE;
        else if (nameStr == "CAMERA_DEVICE_CLOSED")
            return Vision_portal::Camera_state::CAMERA_DEVICE_CLOSED;
        else if (nameStr == "ERROR")
            return Vision_portal::Camera_state::ERROR;
        else
            throw invalid_argument("Invalid enum value");
    }

    void Vision_portal::save_next_frame_raw(const string &var1) const {
        attach_thread
        jstring jvar1 = env->NewStringUTF(var1.c_str());
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "saveNextFrameRaw",
                                             "(Ljava/lang/String;)V"),
                            jvar1);
        env->DeleteLocalRef(jvar1);
    }

    void Vision_portal::stop_streaming() const {
        attach_thread
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "stopStreaming",
                                             "()V"));
    }

    void Vision_portal::resume_streaming() const {
        attach_thread
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "resumeStreaming",
                                             "()V"));
    }

    void Vision_portal::stop_live_view() const {
        attach_thread
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "stopLiveView",
                                             "()V"));
    }

    void Vision_portal::resume_live_view() const {
        attach_thread
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "resumeLiveView",
                                             "()V"));
    }

    float Vision_portal::get_fps() const {
        attach_thread
        return static_cast<float>(env->CallFloatMethod(visionPortal,
                                                       env->GetMethodID(VisionPortal, "getFps",
                                                                        "()F")));
    }

    void Vision_portal::set_active_camera(const Webcam_name &webcam) const {
        attach_thread
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "setActiveCamera",
                                             "(Lorg/firstinspires/ftc/vision/WebcamName;)V"),
                            webcam.webcamName);
    }

    Webcam_name Vision_portal::get_active_camera() const {
        attach_thread
        jobject jwebcamName = env->CallObjectMethod(visionPortal,
                                                    env->GetMethodID(VisionPortal,
                                                                     "getActiveCamera",
                                                                     "()Lorg/firstinspires/ftc/vision/WebcamName;"));
        Webcam_name webcamName(jwebcamName);
        env->DeleteLocalRef(jwebcamName);
        return webcamName;
    }

    void Vision_portal::close() const {
        attach_thread
        env->CallVoidMethod(visionPortal,
                            env->GetMethodID(VisionPortal, "close",
                                             "()V"));
    }


    Vision_portal::Builder::Builder() {
        attach_thread
        jmethodID constructor = env->GetMethodID(VisionPortal_Builder, "<init>", "()V");
        jobject localBuilder = env->NewObject(VisionPortal_Builder, constructor);
        builder = env->NewGlobalRef(localBuilder);
        env->DeleteLocalRef(localBuilder);
    }

    Vision_portal::Builder &Vision_portal::Builder::set_camera(const Camera_name &camera) {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setCamera",
                                                                   "(Lorg/firstinspires/ftc/vision/CameraName;)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  camera.cameraName));

        return *this;
    }

    Vision_portal::Builder &Vision_portal::Builder::set_stream_format(Stream_format streamFormat) {
        attach_thread
        jobject jstreamFormat = env->GetStaticObjectField(VisionPortal,
                                                          env->GetStaticFieldID(VisionPortal,
                                                                                stream_format_to_string(
                                                                                        streamFormat),
                                                                                "Lorg/firstinspires/ftc/vision/VisionPortal$StreamFormat;"));
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setStreamFormat",
                                                                   "(Lorg/firstinspires/ftc/vision/VisionPortal$StreamFormat;)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  jstreamFormat));

        env->DeleteLocalRef(jstreamFormat);
        return *this;
    }

    Vision_portal::Builder &Vision_portal::Builder::enable_live_view(bool enable_live_view) {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "enableLiveView",
                                                                   "(Z)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  enable_live_view));
        return *this;
    }

    Vision_portal::Builder &Vision_portal::Builder::set_auto_stop_live_view(bool auto_pause) {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setAutoStopLiveView",
                                                                   "(Z)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  auto_pause));
        return *this;
    }

    Vision_portal::Builder &
    Vision_portal::Builder::set_live_view_container_id(int live_view_container_id) {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setLiveViewContainerId",
                                                                   "(I)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  static_cast<jint>(live_view_container_id)));
        return *this;
    }

    Vision_portal::Builder &Vision_portal::Builder::set_camera_resolution(Size camera_resolution) {
        attach_thread
        jobject jcameraResolution = env->NewObject(env->FindClass("android/util/Size"),
                                                   env->GetMethodID(
                                                           env->FindClass("android/util/Size"),
                                                           "<init>", "(II)V"),
                                                   static_cast<jint>(camera_resolution.width),
                                                   static_cast<jint>(camera_resolution.height));
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setCameraResolution",
                                                                   "(Lorg/firstinspires/ftc/vision/VisionPortal$Builder$Size;)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  jcameraResolution));
        env->DeleteLocalRef(jcameraResolution);
        return *this;
    }

    Vision_portal::Builder &
    Vision_portal::Builder::add_processor(const Vision_processor &processor) {
        this->processors.emplace_back(processor, true);
        return *this;
    }

    Vision_portal::Builder &Vision_portal::Builder::add_processors(
            const vector <Vision_processor> &processors) {
        for (const auto &processor: processors)
            this->processors.emplace_back(processor, true);
        return *this;
    }

    Vision_portal::Builder &
    Vision_portal::Builder::set_auto_start_stream_on_build(bool auto_start_stream_on_build) {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setAutoStartStreamOnBuild",
                                                                   "(Z)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  auto_start_stream_on_build));
        return *this;
    }

    Vision_portal::Builder &
    Vision_portal::Builder::set_show_stats_overlay(bool show_stats_overlay) {
        attach_thread
        env->DeleteLocalRef(env->CallObjectMethod(builder,
                                                  env->GetMethodID(VisionPortal_Builder,
                                                                   "setShowStatsOverlay",
                                                                   "(Z)Lorg/firstinspires/ftc/vision/VisionPortal$Builder;"),
                                                  show_stats_overlay));
        return *this;
    }

    Vision_portal Vision_portal::Builder::build() {

        attach_thread
        jobject jvisionPortal = env->CallObjectMethod(builder,
                                                      env->GetMethodID(VisionPortal_Builder,
                                                                       "build",
                                                                       "()Lorg/firstinspires/ftc/vision/VisionPortal;"));
        Vision_portal vision_portal(jvisionPortal);
        vision_portal.processors = processors;
        env->DeleteLocalRef(jvisionPortal);
        return vision_portal;
    }
} // sdk