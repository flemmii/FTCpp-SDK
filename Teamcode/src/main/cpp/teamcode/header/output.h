#ifndef FTC_FROG_OUTTAKE_H
#define FTC_FROG_OUTTAKE_H

#include "sdk/hardware/Servo.h"
#include "sdk/hardware/hardware_map.h"

namespace output {
    void init();

    void release_pixel();

    void retract_servo();

    void deactivate();
}

#endif // FTC_FROG_OUTTAKE_H
