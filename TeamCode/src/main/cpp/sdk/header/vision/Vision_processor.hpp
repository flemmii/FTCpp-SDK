#pragma once

#include <opencv2/opencv.hpp>

#include "utils.hpp"

namespace sdk
{
    class Vision_processor
    {
    private:
        static int last_id;
        int id = -1;

    public:
        Vision_processor();

        virtual void init(const int &width, const int &height) const;

        virtual void process_frame(const cv::Mat &input, const long &capture_time_nanos) const;

        // Just draw on frame_to_draw_on using the methods given by OpenCV
        virtual void on_draw_frame(const int &onscreen_width, const int &onscreen_height,
                                   cv::Mat &frame_to_draw_on) const;

        friend bool operator==(const Vision_processor &lhs, const Vision_processor &rhs);
    };

    namespace vision_processor
    {
        extern bool dual_cam_view;

        namespace first_vision_processor
        {
            extern jclass jclazz;
            extern const std::vector<std::pair<sdk::Vision_processor *, bool>> *processors;
        }

        namespace second_vision_processor
        {
            extern jclass jclazz;
            extern const std::vector<std::pair<sdk::Vision_processor *, bool>> *processors;
        }

        void load_native_methods();
    } // vision_processor
} // sdk
