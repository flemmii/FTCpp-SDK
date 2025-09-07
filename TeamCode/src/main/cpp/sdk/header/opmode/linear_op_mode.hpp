#pragma once

#include "utils.hpp"
#include "../hardware/hardware_map.hpp"
#include "hardware/Gamepad.hpp"
#include "../telemetry.hpp"

namespace sdk::linear_op_mode
{
    extern jclass jclazz;
    extern jobject thiz;

    void wait_for_start();

    void idle();

    void sleep(const int &milliseconds);

    bool op_mode_is_active();

    bool op_mode_in_init();

    bool is_started();

    bool is_stop_requested();

    void init();

    void init_loop();

    void start();

    void loop();

    void stop();

    // OpMode
    void terminate_op_mode_now();

    double get_runtime();

    void reset_runtime();

    // OpMode Internal
    void request_op_mode_stop();
}
