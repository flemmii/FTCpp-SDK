//
// Created by Frosch PC - V2 on 04.06.2024.
//

#include "tests/TeamPropDetection.h"

using namespace std;
using namespace sdk::linear_op_mode;
using namespace sdk;
using namespace cv;
using namespace team_prop_detection;

chrono::high_resolution_clock::time_point last_time_millis = chrono::high_resolution_clock::now();
chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();
int loop_time;
vector<int> loop_times;
int min_loop_time = 1000000000;
int max_loop_time = 0;
double avarage_loop_time;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_tests_cpp_TeamPropDetectionCpp_opMode(JNIEnv *env,
                                                                      jobject thiz) {
    init_sdk

    Team_prop_detection detection;
    Vision_portal portal = Vision_portal::easy_create_with_defaults({hardware_map::get(WebcamName, "front_webcam")}, {detection});
    portal.set_processor_enabled(detection, false);

    while (portal.get_camera_state() != Vision_portal::Camera_state::STREAMING);

    telemetry::add_line("Initialized");
    telemetry::update();

    wait_for_start();

    start_time = chrono::high_resolution_clock::now();
    last_time_millis = chrono::high_resolution_clock::now();
    portal.set_processor_enabled(detection, true);

    while (!is_stop_requested() && duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_time).count() < 60000);
    portal.set_processor_enabled(detection, false);
    while (!is_stop_requested());

    stop();
}

namespace team_prop_detection {
    int position_team_prop = 0;
    bool no_team_prop_detected = true;
}

vector<Point> largest_contour;
vector<Point> second_largest_contour;

Scalar largest_contour_color(255, 255, 255);
Scalar second_largest_contour_color(255, 255, 255);
Scalar arrow_color(255, 255, 255);

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

void Team_prop_detection::process_frame(const Mat &input, long capture_time_nanos) const {

    Mat hsv;
    cvtColor(input, hsv, COLOR_RGB2HSV);

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
    } else {
        no_team_prop_detected = true;
        // It will use the last detected position
    }

    // Calculating loop times
    loop_time = chrono::duration_cast<chrono::microseconds>(
            chrono::high_resolution_clock::now() - last_time_millis).count();
    last_time_millis = chrono::high_resolution_clock::now();

    loop_times.push_back(loop_time);
    avarage_loop_time = accumulate(loop_times.begin(), loop_times.end(), 0) / (double) (loop_times.size());

    if (loop_time > max_loop_time)
        max_loop_time = loop_time;

    if (loop_time < min_loop_time)
        min_loop_time = loop_time;

    telemetry::add_data("Loop time", loop_time);
    telemetry::add_data("Loop times size", (double) (loop_times.size()));
    telemetry::add_data("Avarage loop time", avarage_loop_time);
    telemetry::add_data("Max loop time", max_loop_time);
    telemetry::add_data("Min loop time", min_loop_time);
    telemetry::update();
}

void Team_prop_detection::on_draw_frame(int onscreen_width, int onscreen_height,
                                        cv::Mat frame_to_draw_on) const {
    if (!largest_contour.empty()) {
        drawContours(frame_to_draw_on, vector<vector<Point>>{largest_contour}, -1,
                     largest_contour_color, 5);

        if (!second_largest_contour.empty())
            drawContours(frame_to_draw_on, vector<vector<Point>>{second_largest_contour}, -1,
                         second_largest_contour_color, 5);
    }
    arrowedLine(frame_to_draw_on, {100, 120}, {0, 120}, arrow_color, 5);
}