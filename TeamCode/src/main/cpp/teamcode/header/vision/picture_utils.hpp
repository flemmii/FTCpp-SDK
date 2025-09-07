#pragma once

#include "opencv2/opencv.hpp"

namespace picture_utils
{
    cv::Mat normalize_value(const cv::Mat &hsv);

    cv::Mat cut_off_outer_pixel(const cv::Mat &input);

    cv::Mat filter_out_large_objects(const cv::Mat &input);

    double
    measure_distance(double object_mm, int object_pixels);
} // picture_utils
