//
// Created by fnlg on 09.05.2024.
//

#ifndef FTCROBOTCONTROLLER_VISION_PROCESSOR_H
#define FTCROBOTCONTROLLER_VISION_PROCESSOR_H

#include <opencv2/opencv.hpp>

#include "utils.h"

namespace sdk {
    class Vision_processor {
    private:
        static int last_id;
        int id = -1;
    public:
        Vision_processor();

        virtual void init(int width, int height) const;

        virtual void process_frame(const cv::Mat &input, long capture_time_nanos) const;

        // Just draw on frame_to_draw_on using the methods given by OpenCV
        virtual void
        on_draw_frame(int onscreen_width, int onscreen_height, cv::Mat frame_to_draw_on) const;

        friend bool operator==(const Vision_processor &lhs, const Vision_processor &rhs);
    };
} // sdk

namespace vision_processor {
    extern jclass FirstVisionProcessor;
    extern jclass SecondVisionProcessor;

    extern bool dual_cam_view;

    namespace first_vision_processor {
        extern const std::vector<std::pair<sdk::Vision_processor *, bool>> *processors;
    }

    namespace second_vision_processor {
        extern const std::vector<std::pair<sdk::Vision_processor *, bool>> *processors;
    }
} // vision_processor

#endif //FTCROBOTCONTROLLER_VISION_PROCESSOR_H
