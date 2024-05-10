//
// Created by fnlg on 01.01.2024.
//

#include "../header/track_position.h"

using namespace std;
using namespace sdk;

namespace track_position {

    sdk::Dc_motor_ex deadWheelForwards;
    sdk::Dc_motor_ex deadWheelSidewards;

    int encoder_forward_pos = 0;
    int encoder_sidewards_pos = 0;

    double y_position = 0;
    double x_position = 0;
    double rotation_rad = 0;
    double rotation_deg = 0;
    double rotation_deg_continuous = 0;
    double extra_rad = 0;
    int extra_degrees = 0;

    chrono::high_resolution_clock::time_point last_time_millis = chrono::high_resolution_clock::now();
    int loop_time = 0;
    // TODO: adjust n
    // The higher n, the smoother the speeds are. The lower n the better the speeds can react to rapid changes
    vector<double> ring_buffer_x_speed(5, 0);
    vector<double> ring_buffer_y_speed(5, 0);

    double y_speed = 0;
    double x_speed = 0;

    void init() {
        // These variables need to be reset because they get used before they are initialised
        // or they only get changed by external influences such as setter
        encoder_forward_pos = 0;
        encoder_sidewards_pos = 0;

        y_position = 0;
        x_position = 0;
        rotation_deg = 0;
        rotation_deg_continuous = 0;
        extra_rad = 0;
        extra_degrees = 0;

        deadWheelForwards = hardware_map::get(DcMotorEx, "rear_left");
        deadWheelSidewards = hardware_map::get(DcMotorEx, "front_right");

        deadWheelForwards.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
        deadWheelSidewards.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);

        deadWheelForwards.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
        deadWheelSidewards.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);

        thread([] {
            while (!sdk::linear_op_mode::is_stop_requested()) {

                // Getting all the data of the hubs
                robot::control_hub.get_bulk_data();
                if (robot::control_hub.is_not_responding()) {
                    logcat_log(ANDROID_LOG_FATAL, "control hub not responding");
                }

                robot::expansion_hub.get_bulk_data();
                if (robot::expansion_hub.is_not_responding()) {
                    logcat_log(ANDROID_LOG_FATAL, "expansion hub not responding");
                }

                // Writing all the necessary logged data from the loop before into variables
                double previous_rotation_deg = rotation_deg;
                double previous_rotation_deg_continuous = rotation_deg_continuous;
                int previous_encoder_forward_pos = encoder_forward_pos;
                int previous_encoder_sidewards_pos = encoder_sidewards_pos;

                // Getting all the new data for tracking
                rotation_rad = -robot::imu.get_rotation().z + extra_rad;
                encoder_forward_pos = -deadWheelForwards.get_current_position();
                encoder_sidewards_pos = deadWheelSidewards.get_current_position();
                double delta_t = (double) (chrono::duration_cast<chrono::milliseconds>(
                        chrono::high_resolution_clock::now() - last_time_millis).count()) / 1000;

                // Calculating loop time, this is just nice to see c: also storing the current time
                loop_time = (int) (delta_t * 1000);
                last_time_millis = chrono::high_resolution_clock::now();

                // Converting radians to degrees and continuous degree system
                rotation_deg = (rotation_rad * 180) / M_PI;
                if (previous_rotation_deg - rotation_deg > 180) {
                    extra_degrees += 360;
                } else if (previous_rotation_deg - rotation_deg < -180) {
                    extra_degrees -= 360;
                }
                rotation_deg_continuous = rotation_deg + extra_degrees;

                // Calculating the delta values relative to the robots facing
                double delta_forward =
                        ((double) (previous_encoder_forward_pos - encoder_forward_pos) /
                         ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM * M_PI;
                double delta_sidewards =
                        ((double) (previous_encoder_sidewards_pos - encoder_sidewards_pos) /
                         ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM * M_PI;
                double delta_rotation =
                        (previous_rotation_deg_continuous - rotation_deg_continuous) * (M_PI / 180);

                // This calculates how the deltas are if we got high rotation speed
                // Because if we got high rotation speed we'll rather drive a curve than a line
                // TODO: This value needs to be adjusted
                if (fabs(delta_rotation) > 3) {
                    double radius_forward = delta_forward / delta_rotation;
                    double radius_sidewards = delta_sidewards / delta_rotation;

                    double sinus_delta_rot = sin(delta_rotation);
                    double cosinus_delta_rot = cos(delta_rotation);

                    delta_forward = radius_forward * sinus_delta_rot +
                                    radius_sidewards * (1 - cosinus_delta_rot);
                    delta_sidewards = radius_forward * (1 - cosinus_delta_rot) -
                                      radius_sidewards * sinus_delta_rot;
                }

                // Calculating the way we drove in x and y direction since the last loop
                double sinus = sin(rotation_rad);
                double cosinus = cos(rotation_rad);
                double delta_y = delta_forward * cosinus + delta_sidewards * sinus;
                double delta_x = delta_forward * sinus - delta_sidewards * cosinus;

                // Adding this way to the current position -> new position
                y_position += delta_y;
                x_position += delta_x;

                // Converting the deltas into the speeds
                delta_y /= delta_t;
                delta_x /= delta_t;

                // Stores the last n speeds in a ring buffer (n is defined at the declaration of the ring buffers)
                ring_buffer_x_speed.insert(ring_buffer_x_speed.begin(), delta_x);
                ring_buffer_y_speed.insert(ring_buffer_y_speed.begin(), delta_y);

                // Deletes the last element
                ring_buffer_x_speed.pop_back();
                ring_buffer_y_speed.pop_back();

                // Calculates the speeds through adding up all the last n calculated speeds
                // and then dividing them through n. This way the speeds get normalized and
                // we don't get completely wrong values
                y_speed = accumulate(ring_buffer_y_speed.begin(), ring_buffer_y_speed.end(), 0.0) /
                          ring_buffer_y_speed.size();
                x_speed = accumulate(ring_buffer_x_speed.begin(), ring_buffer_x_speed.end(), 0.0) /
                          ring_buffer_x_speed.size();
            }
        }).detach();
    }

    void set_position(double x_position, double y_position, double rotation) {
        track_position::x_position = x_position;
        track_position::y_position = y_position;
        extra_degrees = 360 * (int) (rotation > 0 ? floor(rotation / 360) : ceil(rotation / 360));

        rotation = fmod(rotation, 360);
        if (rotation > 180) {
            rotation -= 360;
        } else if (rotation < -180) {
            rotation += 360;
        }
        extra_rad += rotation * (M_PI / 180) - rotation_rad;
    }

    void set_position(Position position) {
        set_position(position.x, position.y, position.rotation);
    }

    void set_coordinates(double x_position, double y_position) {
        track_position::x_position = x_position;
        track_position::y_position = y_position;
    }

    void set_rotation(double rotation) {
        extra_degrees = 360 * (int) (rotation > 0 ? floor(rotation / 360) : ceil(rotation / 360));

        rotation = fmod(rotation, 360);
        if (rotation > 180) {
            rotation -= 360;
        } else if (rotation < -180) {
            rotation += 360;
        }
        extra_rad += rotation * (M_PI / 180) - rotation_rad;
    }

    Position get_position() {
        return {x_position, y_position, rotation_deg_continuous};
    }

    void log_current_position() {
        telemetry::add_data("x", x_position);
        telemetry::add_data("y", y_position);
        telemetry::add_data("rotation", rotation_deg_continuous);
        telemetry::update();
    }

    Vec2d get_coordinates() {
        return {x_position, y_position};
    }

    double get_forward_mm() {
        return ((double) (encoder_forward_pos) / ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM *
               M_PI;
    }

    double get_sidewards_mm() {
        return ((double) (encoder_sidewards_pos) / ENCODER_TICKS_PER_ROTATION) * WHEEL_DIAMETER_MM *
               M_PI;
    }

    double get_rotation_rad() {
        return rotation_rad;
    }

    Vec2d get_speed() {
        return {x_speed, y_speed};
    }

    double get_extra_degrees() {
        return extra_degrees;
    }

    int get_loop_time() {
        return loop_time;
    }
}