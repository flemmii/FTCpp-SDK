#include "../header/error_detection.h"

using namespace std;

namespace error_detection {
    map<string, int> wrong_velocity_counts;
    map<string, bool> motors_on_fire;

    bool detect_fire(const string &motor_name, double current_value, double target_value,
                     int max_wrong_current_values) {
        bool motor_on_fire = true;
        auto it = motors_on_fire.find(motor_name);
        if (it == motors_on_fire.end() || !it->second) {
            int wrong_velocity_count = 0;
            auto count_it = wrong_velocity_counts.find(motor_name);
            if (count_it != wrong_velocity_counts.end()) {
                wrong_velocity_count = count_it->second;
            }

            motor_on_fire = false;
            if (it == motors_on_fire.end()) {
                motors_on_fire[motor_name] = false;
            }

            if (target_value != 0 && current_value == 0) {
                wrong_velocity_count++;
                if (wrong_velocity_count == max_wrong_current_values) {
                    logcat_log(ANDROID_LOG_ERROR, "%s", (motor_name + ": Encoder damaged").c_str());
                    motor_on_fire = true;
                    motors_on_fire[motor_name] = true;
                }
            } else if (target_value != 0) {
                wrong_velocity_count = 0;
            }
            wrong_velocity_counts[motor_name] = wrong_velocity_count;
        }
        return motor_on_fire;
    }

    bool is_my_motor_on_fire(const std::string &motorName) {
        auto it = motors_on_fire.find(motorName);
        return it != motors_on_fire.end() && it->second;
    }

    std::map<std::string, bool> get_motors_on_fire() {
        return motors_on_fire;
    }

    void print_motor_status() {
        for (const auto &motorPair: motors_on_fire) {
            if (motorPair.second) {
                sdk::telemetry::add_data(motorPair.first, "Encoder is damaged");
            }
        }
    }
}
