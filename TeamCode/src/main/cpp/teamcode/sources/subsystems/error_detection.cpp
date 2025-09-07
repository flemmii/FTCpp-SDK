#include "subsystems/error_detection.hpp"

#include <vector>
#include <string>
#include <chrono>

#include "utils.hpp"

using namespace std;

namespace error_detection
{
    vector<string> motor_names;
    vector<int> max_wrong_values;
    vector<int> wrong_value_counts;
    vector<chrono::steady_clock::duration> cooldowns;
    vector<chrono::steady_clock::time_point> last_times_checked;
    vector<bool> motors_on_fire;

    size_t add_motor(const string &motor_name, int max_wrong_values,
                     chrono::steady_clock::duration cooldown)
    {
        const size_t index = wrong_value_counts.size();
        motor_names.push_back(motor_name);
        error_detection::max_wrong_values.push_back(max_wrong_values);
        wrong_value_counts.push_back(0);
        cooldowns.push_back(cooldown);
        last_times_checked.push_back(chrono::steady_clock::now());
        motors_on_fire.push_back(false);
        return index;
    }

    bool detect_fire(size_t index, Tick current_value, Power target_value)
    {
        if (wrong_value_counts.size() <= index)
            return false;

        bool motor_on_fire = motors_on_fire[index];
        chrono::steady_clock::time_point &last_time_checked = last_times_checked[index];
        if (chrono::steady_clock::now() - last_time_checked < cooldowns[index])
            return motor_on_fire;

        if (motor_on_fire)
            return true;

        last_time_checked = chrono::steady_clock::now();
        int &wrong_value_count = wrong_value_counts[index];

        if (target_value != 0_power && current_value == 0_tick)
        {
            wrong_value_count++;
            log(ANDROID_LOG_WARN, "error_detection") << motor_names[index] << ": Wrong Count";
            if (wrong_value_count == max_wrong_values[index])
            {
                motors_on_fire[index] = true;
                motor_on_fire = true;
                log(ANDROID_LOG_ERROR, "error_detection") << motor_names[index] << ": Encoder damaged";
            }
        }
        else if (target_value != 0_power)
        {
            wrong_value_count = 0;
        }

        return motor_on_fire;
    }

    bool is_my_motor_on_fire(size_t index)
    {
        return motors_on_fire[index];
    }

    vector<string> get_motors_on_fire()
    {
        vector<string> result;
        for (size_t i = 0; i < motor_names.size(); i++)
        {
            if (motors_on_fire[i])
                result.push_back(motor_names[i]);
        }
        return result;
    }
}
