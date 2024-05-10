//
// Created by fnlg on 29.04.2024.
//

#ifndef FTCROBOTCONTROLLER_PICTURE_NORMALIZATION_H
#define FTCROBOTCONTROLLER_PICTURE_NORMALIZATION_H

#include "opencv2/opencv.hpp"

#include "../../sdk/header/init_sdk.h"

namespace picture_normalization {
    cv::Mat normalize_value(const cv::Mat &hsv);

    cv::Mat cut_off_outer_pixel(const cv::Mat &input);

    cv::Mat filter_out_large_objects(const cv::Mat &input);
} // picture_normalization

#endif //FTCROBOTCONTROLLER_PICTURE_NORMALIZATION_H
