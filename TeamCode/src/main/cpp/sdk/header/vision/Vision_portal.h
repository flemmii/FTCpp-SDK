//
// Created by Frosch PC - V2 on 05.02.2024.
//

#ifndef FTCROBOTCONTROLLER_VISION_PORTAL_H
#define FTCROBOTCONTROLLER_VISION_PORTAL_H

#include <vector>

#include "utils.h"
#include "../hardware/camera/Camera_name.h"
#include "Vision_processor.h"
#include "../hardware/camera/Webcam_name.h"

namespace sdk {

    class Vision_portal {
    private:
        static int vision_portal_count;
    public:
        static jclass jclazz;
        static jclass jclazz_StreamFormat;
        static jclass jclazz_MultiPortalLayout;

        jobject visionPortal;

        std::vector<std::pair<sdk::Vision_processor *, bool>> processors;

        static int DEFAULT_VIEW_CONTAINER_ID;
        enum class Stream_format {
            YUY2,
            MJPEG
        };

        enum class Multi_portal_layout {
            VERTICAL,
            HORIZONTAL
        };

        enum class Camera_state {
            OPENING_CAMERA_DEVICE,
            CAMERA_DEVICE_READY,
            STARTING_STREAM,
            STREAMING,
            STOPPING_STREAM,
            CLOSING_CAMERA_DEVICE,
            CAMERA_DEVICE_CLOSED,
            ERROR
        };

        struct Size {
            Size(int width, int height) : width(width), height(height) {}

            int width;
            int height;
        };

        static const char *multi_portal_layout_to_string(Vision_portal::Multi_portal_layout mpl);

        static const char *stream_format_to_string(Vision_portal::Stream_format sf);

        Vision_portal(jobject visionPortal);

        ~Vision_portal();

        Vision_portal &operator=(jobject visionPortal);

        [[nodiscard]] static std::vector<int>
        makeMultiPortalView(int numPortals, Multi_portal_layout mpl);

        [[nodiscard]] static Vision_portal easy_create_with_defaults(const Camera_name &camera_name,
                                                                     const std::vector<sdk::Vision_processor *> &processors = {});

        static void enable_dual_cam_view();

        static void disable_dual_cam_view();

        void add_processor(sdk::Vision_processor *processor);

        void add_processors(const std::vector<sdk::Vision_processor *> &processors);

        void remove_processor(const sdk::Vision_processor *processor);

        void remove_processors(const std::vector<sdk::Vision_processor *> &processors);

        void set_processor_enabled(const sdk::Vision_processor *processor,
                                   bool enabled);

        [[nodiscard]] bool get_processor_enabled(const sdk::Vision_processor *processor);

        [[nodiscard]] Camera_state get_camera_state() const;

        void save_next_frame_raw(const std::string &var1) const;

        void stop_streaming() const;

        void resume_streaming() const;

        void stop_live_view() const;

        void resume_live_view() const;

        [[nodiscard]] float get_fps() const;

        void set_active_camera(const Webcam_name &webcam) const;

        [[nodiscard]] Webcam_name get_active_camera() const;

        void close() const;


        class Builder {
        private:
            std::vector<std::pair<sdk::Vision_processor *, bool>> processors;

        public:
            static jclass jclazz;

            jobject builder;

            Builder();

            ~Builder();

            Builder &operator=(jobject builder);

            Builder &set_camera(const Camera_name &camera);

            Builder &set_stream_format(Stream_format stream_format);

            Builder &enable_live_view(bool enableLiveView);

            Builder &set_auto_stop_live_view(bool auto_pause);

            Builder &set_live_view_container_id(int live_view_container_id);

            Builder &set_camera_resolution(Size cameraResolution);

            Builder &add_processor(sdk::Vision_processor *processor);

            Builder &
            add_processors(const std::vector<sdk::Vision_processor *> &processors);

            Builder &set_auto_start_stream_on_build(bool auto_start_stream_on_build);

            Builder &set_show_stats_overlay(bool show_stats_overlay);

            Vision_portal build();
        };
    };

} // sdk

#endif //FTCROBOTCONTROLLER_VISION_PORTAL_H
