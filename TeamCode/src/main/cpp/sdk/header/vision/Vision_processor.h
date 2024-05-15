//
// Created by fnlg on 09.05.2024.
//

#ifndef FTCROBOTCONTROLLER_VISION_PROCESSOR_H
#define FTCROBOTCONTROLLER_VISION_PROCESSOR_H

#include <jni.h>

#include "opencv2/opencv.hpp"

namespace sdk {
    class Vision_processor {
    private:
        static int last_id;
        int id = -1;
    public:
        Vision_processor();

        virtual void init(int width, int height) const;

        [[nodiscard]] virtual cv::Mat
        process_frame(const cv::Mat &input, long capture_time_nanos) const;

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
