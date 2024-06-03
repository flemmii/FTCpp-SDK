//
// Created by fnlg on 29.12.2023.
//

#ifndef FTCROBOTCONTROLLER_LINEAR_OP_MODE_H
#define FTCROBOTCONTROLLER_LINEAR_OP_MODE_H

#include "utils.h"
#include "../hardware/hardware_map.h"
#include "hardware/Gamepad.h"
#include "../telemetry.h"

namespace sdk {
    extern jobject currentOpMode;
    extern jclass CurrentOpMode;

    namespace linear_op_mode {
        // LinearOpMode
        void wait_for_start();

        void idle();

        void sleep(int milliseconds);

        bool op_mode_is_active();

        bool op_mode_in_init();

        bool is_started();

        bool is_stop_requested();

        void init();

        void init_loop();

        void start();

        void loop();

        void stop();
    }
}


#endif //FTCROBOTCONTROLLER_LINEAR_OP_MODE_H
