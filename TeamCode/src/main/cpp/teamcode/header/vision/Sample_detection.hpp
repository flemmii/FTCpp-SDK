#pragma once

#include "vision/Vision_processor.hpp"
#include "vision/Vision_portal.hpp"
#include "new_detection.hpp"
#include <semaphore.h>

constexpr bool imageRec_debug_mode_enabled = false;

class Sample_detection : public sdk::Vision_processor
{
public:
    Sample_detection();
    ~Sample_detection();

    void process_frame(const cv::Mat &input, const long &capture_time_nanos) const override;

    void
    on_draw_frame(const int &onscreen_width, const int &onscreen_height,
                  cv::Mat &frame_to_draw_on) const override;

    tuple<bool, Vec2d<false>, Degree> get_sample_position(int tries = 3, bool disable_yellow = false);

    // 16:9
    sdk::Vision_portal::Size resolution = {640, 360};
    void init();

private:
    mutable bool disable_yellow;
    mutable int tries;
    mutable sem_t sem;
    mutable mutex sample_position_mutex;
    mutable tuple<bool, Vec2d<false>, Degree> sample_position;
    mutable mutex this_mutex;
    mutable cv::Mat shared_canvas;
};
