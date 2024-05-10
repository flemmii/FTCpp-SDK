#ifndef FTC_FROG_INTAKE_H
#define FTC_FROG_INTAKE_H

#include "sdk/hardware/Dc_motor_ex.h"
#include "sdk/hardware/hardware_map.h"

namespace intake {

    void init();

    void activate_intake();

    void deactivate_intake();

    void activate_intake_reversed();

    void set_power(double power);
}

#endif // FTC_FROG_INTAKE_H
