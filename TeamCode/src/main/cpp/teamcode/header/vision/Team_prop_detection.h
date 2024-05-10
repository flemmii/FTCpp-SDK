//
// Created by fnlg on 29.04.2024.
//

#ifndef FTCROBOTCONTROLLER_TEAM_PROP_DETECTION_H
#define FTCROBOTCONTROLLER_TEAM_PROP_DETECTION_H

#include <opencv2/opencv.hpp>

#include "sdk/vision/Vision_processor.h"

#include "../../sdk/header/init_sdk.h"
#include "../match_data.h"
#include "picture_normalization.h"

/*
 * This is NOT a comment. It tells the compiler to ignore files 
 * from automatic java file creation
 * @Disabled
 */

namespace team_prop_detection {
    extern int position_team_prop;
    extern bool no_team_prop_detected;
}

class Team_prop_detection : public sdk::Vision_processor {
public:
    [[nodiscard]] cv::Mat
    process_frame(const cv::Mat &input, long capture_time_nanos) const override;
}; // vision

#endif //FTCROBOTCONTROLLER_TEAM_PROP_DETECTION_H
