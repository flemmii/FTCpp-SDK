//
// Created by Frosch PC - V2 on 04.06.2024.
//

#ifndef FTCROBOTCONTROLLER_TEAMPROPDETECTION_H
#define FTCROBOTCONTROLLER_TEAMPROPDETECTION_H

#include <numeric>

#include "sdk.hpp"

/*
 * This is NOT a comment. It tells the compiler how to create the java files
 * @TeleOp(name = "TeamPropDetectionCpp", group = "tests")
 */
extern "C" void TeamPropDetectionCpp();

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
};

#endif // FTCROBOTCONTROLLER_TEAMPROPDETECTION_H