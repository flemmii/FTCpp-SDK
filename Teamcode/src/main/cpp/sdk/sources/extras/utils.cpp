//
// Created by fnlg on 09.05.2024.
//

#include "extras/utils.h"

using namespace std;

void logcat_log(int ERROR_LEVEL, const char *text, ...) {
    va_list args;
    va_start(args, text);
    __android_log_vprint(ERROR_LEVEL, "HardwareIssues", text, args);
    va_end(args);
}

void sleep(const chrono::duration<double> &duration) {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + duration;

    while (chrono::high_resolution_clock::now() < end && !sdk::linear_op_mode::is_stop_requested());
}

namespace sdk {
    JavaVM *jvm;
}