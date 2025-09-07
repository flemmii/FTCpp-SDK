#pragma once

#include <opencv2/opencv.hpp>

#include "vision/Vision_processor.hpp"

#include "init_sdk.hpp"
#include "match_data.hpp"
#include "picture_utils.hpp"

/*
 * This is NOT a comment. It tells the compiler to ignore files
 * from automatic java file creation
 * @Disabled
 */

namespace team_prop_detection
{
    extern int position_team_prop;
    extern bool no_team_prop_detected;
}

class Team_prop_detection : public sdk::Vision_processor
{
public:
    void process_frame(const cv::Mat &input, const long &capture_time_nanos) const override;

    void
    on_draw_frame(const int &onscreen_width, const int &onscreen_height,
                  cv::Mat &frame_to_draw_on) const override;
}; // vision
