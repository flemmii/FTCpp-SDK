//
// Created by fnlg on 29.04.2024.
//

#include "../../header/vision/Team_prop_detection.h"

using namespace std;
using namespace sdk;
using namespace cv;
using namespace team_prop_detection;

namespace team_prop_detection {
    int position_team_prop = 0;
    bool no_team_prop_detected = true;
}

vector<Point> largest_contour;
vector<Point> second_largest_contour;

#if red_alliance
Scalar low_hsv1(0, 80, 50);
Scalar high_hsv1(15, 255, 255);
Scalar low_hsv2(165, 80, 50);
Scalar high_hsv2(180, 255, 255);

Scalar alliance_color(255, 0, 0);
#else
Scalar low_hsv1(90, 80, 80);
Scalar high_hsv1(120, 255, 255);

Scalar alliance_color(0, 0, 255);
#endif

Mat Team_prop_detection::process_frame(const Mat &input, long capture_time_nanos) const {
    Mat corrected_input = picture_normalization::filter_out_large_objects(
            picture_normalization::cut_off_outer_pixel(input));

    Mat hsv;
    cvtColor(corrected_input, hsv, COLOR_RGB2HSV);
    hsv = picture_normalization::normalize_value(hsv);

    Mat final_thresh;
#if red_alliance
    Mat thresh1;
    Mat thresh2;

    inRange(hsv, low_hsv1, high_hsv1, thresh1);
    inRange(hsv, low_hsv2, high_hsv2, thresh2);
    bitwise_or(thresh1, thresh2, final_thresh);
#else
    inRange(hsv, low_hsv1, high_hsv1, final_thresh);
#endif

    vector<vector<Point>> contours{};
    findContours(final_thresh, contours, OutputArray{},
                 RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    double largest_area = -1;
    double second_largest_area = -1;

    for (auto &contour: contours) {
        double area = contourArea(contour);
        if (area > second_largest_area) {
            if (area > largest_area) {
                second_largest_area = largest_area;
                second_largest_contour = largest_contour;

                largest_area = area;
                largest_contour = contour;
            } else {
                second_largest_area = area;
                second_largest_contour = contour;
            }
        }
    }

    Point mid_largest_contour;
    Point mid_second_largest_contour;
    vector<Point> approx_curve_largest_contour;
    vector<Point> approx_curve_second_largest_contour{};

    if (!largest_contour.empty()) {
        no_team_prop_detected = false;

        double epsilon = 0.02 * arcLength(largest_contour, true);
        approxPolyDP(largest_contour, approx_curve_largest_contour, epsilon, true);

        Moments moments = cv::moments(largest_contour);
        mid_largest_contour = {(int) (moments.m10 / moments.m00),
                               (int) (moments.m01 / moments.m00)};

        if (!second_largest_contour.empty()) {
            epsilon = 0.02 * arcLength(second_largest_contour, true);
            approxPolyDP(second_largest_contour, approx_curve_second_largest_contour, epsilon,
                         true);

            moments = cv::moments(second_largest_contour);
            mid_second_largest_contour = {(int) (moments.m10 / moments.m00),
                                          (int) (moments.m01 / moments.m00)};
        }

        Scalar largest_contour_color(255, 255, 255);
        Scalar second_largest_contour_color(255, 255, 255);
        Scalar arrow_color(255, 255, 255);

        if (approx_curve_largest_contour.size() > 5 && !largest_contour.empty()) {
            largest_contour_color = alliance_color;
            if (mid_largest_contour.x < mid_second_largest_contour.x) {
                position_team_prop = 1;
            } else {
                position_team_prop = 2;
            }
        } else if (approx_curve_second_largest_contour.size() > 5 &&
                   !second_largest_contour.empty()) {
            second_largest_contour_color = alliance_color;

            if (mid_second_largest_contour.x < mid_largest_contour.x) {
                position_team_prop = 1;
            } else {
                position_team_prop = 2;
            }
        } else {
            arrow_color = alliance_color;
            position_team_prop = 0;
        }

        Mat final_mat = corrected_input.clone();

        if (!largest_contour.empty()) {
            drawContours(final_mat, vector<vector<Point>>{largest_contour}, -1,
                         largest_contour_color, 5);

            if (!second_largest_contour.empty())
                drawContours(final_mat, vector<vector<Point>>{second_largest_contour}, -1,
                             second_largest_contour_color, 5);
        }
        arrowedLine(final_mat, {100, 120}, {0, 120}, arrow_color, 5);
        return final_mat;
    } else {
        no_team_prop_detected = true;
        return input;
        // It will use the last detected position
    }
}
// vision
