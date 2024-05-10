#ifndef FTC_FROG_SUSPENSION_ARM_H
#define FTC_FROG_SUSPENSION_ARM_H

#include "sdk/hardware/Dc_motor_ex.h"
#include "sdk/hardware/Servo.h"
#include "sdk/hardware/hardware_map.h"
#include "error_detection.h"

namespace suspension_arm_and_drone_launcher {
    void init();

    void set_target_power(double power);

    void set_target_elevation_in_degrees(double elevation_in_degrees, double power = 1);

    void check_encoder();

    void use_encoder(bool use_encoder);

    double get_elevation_in_degrees();

    double get_target_elevation_in_degrees();

    double get_max_elevation();

    void stop();

    bool is_at_target_position();

    void launch_drone();

    void test();
}
#endif //FTC_FROG_SUSPENSION_ARM_H
