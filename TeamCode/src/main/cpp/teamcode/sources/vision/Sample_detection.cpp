#include "vision/Sample_detection.hpp"
#include "configurations/all_configurations.hpp"
#include "subsystems/intake/intake.hpp"
#include "utils/serialisation.hpp"
#include "utils/rotation.hpp"
#include "match_data.hpp"
#include "robot.hpp"
#include <fstream>
#include <tuple>

#include "utils.hpp"

tuple<bool, Vec2d<false>, Degree> sample_position = {false, {0_mm, 0_mm}, 0_deg};
mutex sample_position_mutex;

Sample_detection::Sample_detection()
{
    sem_init(&sem, 0, 0);
}

Sample_detection::~Sample_detection()
{
    sem_destroy(&sem);
}

// Note color space might be rgb here but bgr is expected
void Sample_detection::process_frame(const cv::Mat &input, const long &capture_time_nanos) const
{
    sample_position = {false, {0_mm, 0_mm}, 0_deg};

    cv::Mat local_shared_canvas = input.clone();
    if (local_shared_canvas.empty())
        return;
    Millimetre cam_height = intake::get_camera_height();
    Degree cam_angle = intake::get_camera_angle();

    if constexpr (imageRec_debug_mode_enabled)
    {
        saveMatToBitmap(local_shared_canvas, "/sdcard/frame.bmp", true);
        log(ANDROID_LOG_DEBUG, "image_rec") << "cam height: " << cam_height << endl;
        log(ANDROID_LOG_DEBUG, "image_rec") << "cam angle: " << cam_angle << endl;

        string log_file_path = "/sdcard/camera_info.txt";
        std::ofstream log_file(log_file_path, std::ios::app); // Open in append mode
        std::ofstream clear_file(log_file_path, std::ios::trunc);
        clear_file.close();
        if (log_file.is_open())
        {
            log_file << "cam height: " << cam_height << std::endl;
            log_file << "cam angle: " << cam_angle << std::endl;
            log_file.close();
        }
    }

    all_configurations::init();
    all_configurations::camera_configuration = Camera_configuration::construct_logitech_webcam(cam_height, cam_angle, this->resolution.width, this->resolution.height);

    //---hues---
    // red low
    all_configurations::filter_configurations.low_red.hue_lower_bound = 0;
    all_configurations::filter_configurations.low_red.hue_upper_bound = 10;
    // red high
    all_configurations::filter_configurations.high_red.hue_lower_bound = 150;
    all_configurations::filter_configurations.high_red.hue_upper_bound = 250;
    // yellow
    all_configurations::filter_configurations.yellow.hue_lower_bound = 17;
    all_configurations::filter_configurations.yellow.hue_upper_bound = 35;
    // blue
    all_configurations::filter_configurations.blue.hue_lower_bound = 108;
    all_configurations::filter_configurations.blue.hue_upper_bound = 122;

    //---Saturation---
    all_configurations::filter_configurations.low_red.saturation_threshold = 55;
    all_configurations::filter_configurations.high_red.saturation_threshold = 55;
    all_configurations::filter_configurations.yellow.saturation_threshold = 60;
    all_configurations::filter_configurations.blue.saturation_threshold = 30;

    //---HSV---
    // min hsv
    all_configurations::filter_configurations.low_red.min_hsv_value = 60;
    all_configurations::filter_configurations.high_red.min_hsv_value = 60;
    all_configurations::filter_configurations.yellow.min_hsv_value = 143;
    all_configurations::filter_configurations.blue.min_hsv_value = 20;
    // max hsv
    all_configurations::filter_configurations.low_red.max_hsv_value = 255;
    all_configurations::filter_configurations.high_red.max_hsv_value = 255;
    all_configurations::filter_configurations.yellow.max_hsv_value = 255;
    all_configurations::filter_configurations.blue.max_hsv_value = 255;

    //---min_saturation_delta---
    all_configurations::filter_configurations.min_saturation_delta = 30;

    // saturation_filter_enabled and normalize_value_enabled
    all_configurations::filter_configurations.saturation_filter_enabled = false;
    all_configurations::filter_configurations.normalize_value_enabled = true;

    all_configurations::miscellaneous_Configurations.maximal_line_vertical_deviation = 20_deg;
    all_configurations::miscellaneous_Configurations.sample_dimension_tolerance = 5_cm;

    // for some reason rotate causes trouble
    rotate_image(local_shared_canvas, local_shared_canvas);
    cv::cvtColor(local_shared_canvas, local_shared_canvas, cv::COLOR_RGB2BGR);
    detect_samples(local_shared_canvas);

    auto samples = camera_view::get_samples_sorted_by_area();

    for (int i = 0; i < samples.size(); i++)
    {
        auto sample = samples[i];

        value_or_failure<Sample::Position_and_rotation> position_and_rotation_result;
        try
        {
            position_and_rotation_result = sample->get_position_and_rotation();
        }
        catch (int e)
        {
            if constexpr (imageRec_debug_mode_enabled)
            {
                log(ANDROID_LOG_DEBUG, "image_rec_err") << "err code: " << e << endl;
            }
            // position_and_rotation_result.emplace<Calculation_Failure>(Calculation_Failure::Unknown);
        }

        auto color = sample->color;
        bool is_right_color = true;
        if (((color == sample_Color::blue) && match_data::current_match::red_alliance))
        {
            is_right_color = false;
        }
        if ((color == sample_Color::red) && !match_data::current_match::red_alliance)
        {
            is_right_color = false;
        }
        if (disable_yellow && (color == sample_Color::yellow))
        {
            is_right_color = false;
        }

        if constexpr (imageRec_debug_mode_enabled)
        {
            log(ANDROID_LOG_DEBUG, "image_rec") << "Wrong color moving on" << color << endl;
        }

        auto pos_and_rot_ptr = std::get_if<Sample::Position_and_rotation>(&position_and_rotation_result);
        if (pos_and_rot_ptr && is_right_color)
        {
            Degree rotation = std::get<Degree>(pos_and_rot_ptr->rotation);
            position_type position = std::get<position_type>(pos_and_rot_ptr->position);
            unique_lock<mutex> lock(sample_position_mutex);
            sample_position = {true, {position.sidewards, position.forwards}, rotation};
            lock.unlock();

            if constexpr (imageRec_debug_mode_enabled)
            {
                sample->draw_outline(local_shared_canvas, cv::Scalar(255, 0, 255), 2);
                log(ANDROID_LOG_DEBUG, "image_rec") << "rotation :" << rotation.Value() << endl;
                log(ANDROID_LOG_DEBUG, "image_rec") << "pos forwards:" << position.forwards.Value() << endl;
                log(ANDROID_LOG_DEBUG, "image_rec") << "pos sidewards :" << position.sidewards.Value() << endl;
            }
            tries = 1;
            break;
        }
        auto failure = std::get_if<Calculation_Failure>(&position_and_rotation_result);
        if (failure)
        {
            if constexpr (imageRec_debug_mode_enabled)
            {
                switch (*failure)
                {
                case Calculation_Failure::Division_By_Zero:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Division By Zero" << endl;
                    break;
                case Calculation_Failure::Insufficient_Input:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Insufficient Input" << endl;
                    break;
                case Calculation_Failure::Insufficient_Input_Not_2_Vertical_Lines:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Insufficient Input Not 2 Vertical Lines" << endl;
                    break;
                case Calculation_Failure::Not_Implemented:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Not Implemented" << endl;
                    break;
                case Calculation_Failure::Invalid_Sample_:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Invalid Sample" << endl;
                    break;
                case Calculation_Failure::Invalid_Sample_Dimensions:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Invalid Sample Dimensions" << endl;
                    break;
                case Calculation_Failure::Too_Many_Neighbours:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Too Many Neighbours" << endl;
                    break;
                case Calculation_Failure::Too_Few_Neighbours:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Too Few Neighbours" << endl;
                    break;
                default:
                    log(ANDROID_LOG_DEBUG, "image_rec") << "Calculation Failure: Unknown Error" << endl;
                    break;
                }
            }
        }
    }

    if constexpr (imageRec_debug_mode_enabled)
    {
        draw_edges(local_shared_canvas, camera_view::edges_blue);
        draw_edges(local_shared_canvas, camera_view::edges_red);
        draw_edges(local_shared_canvas, camera_view::edges_yellow);
    }

    tries--;
    if (tries <= 0)
    {
        sem_post(&sem);
    }

    unique_lock<mutex> lock(this_mutex);
    shared_canvas = local_shared_canvas;
    lock.unlock();
}

void Sample_detection::on_draw_frame(const int &onscreen_width, const int &onscreen_height,
                                     cv::Mat &frame_to_draw_on) const
{
    unique_lock<mutex> lock(this_mutex);
    cv::cvtColor(shared_canvas, frame_to_draw_on, cv::COLOR_RGB2BGR);
    rotate_image_back(frame_to_draw_on, frame_to_draw_on);
    lock.unlock();
}
tuple<bool, Vec2d<false>, Degree> Sample_detection::get_sample_position(int tries, bool disable_yellow)
{
    log(ANDROID_LOG_INFO, "Sample_detection") << "Running sample detection " << (disable_yellow ? "without yellow samples" : "with yellow samples included") << endl;
    this->disable_yellow = disable_yellow;
    this->tries = tries;
    robot::vision_portal.set_processor_enabled(this, true);
    sem_wait(&sem);
    auto pos = sample_position;
    robot::vision_portal.set_processor_enabled(this, false);
    if (get<0>(pos))
        log(ANDROID_LOG_INFO, "Sample_detection") << "Found sample at: " << get<1>(pos) << " with" << get<2>(pos) << endl;
    else
        log(ANDROID_LOG_WARN, "Sample_detection") << "No sample found after " << tries << " tries" << endl;

    return pos;
}