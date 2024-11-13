//
// Created by fnlg on 09.05.2024.
//

#include "utils.h"

using namespace std;

void logcat_log(const int &ERROR_LEVEL, const char *tag, const char *text, ...) {
    va_list args;
    va_start(args, text);
    __android_log_vprint(ERROR_LEVEL, tag, text, args);
    va_end(args);
}

void sleep(const chrono::duration<double> &duration) {
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count();
    int execution_count = static_cast<int>(milliseconds) / 50;
    auto rest_time = chrono::milliseconds(milliseconds % 50);

    for (int i = 0; i < execution_count; i++) {
        if (sdk::linear_op_mode::is_stop_requested())
            return;

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    if (sdk::linear_op_mode::is_stop_requested())
        return;

    this_thread::sleep_for(rest_time);
}

namespace sdk {
    JavaVM *jvm;
}