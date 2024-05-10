#include "../header/intake.h"

using namespace sdk;

namespace intake {
    Dc_motor_ex intake_motor_right;
    Dc_motor_ex intake_motor_left;

    void init() {
        intake_motor_right = hardware_map::get(DcMotorEx, "intake_right");
        intake_motor_right.set_power(0);
        intake_motor_right.set_mode(Dc_motor_ex::Run_mode::RUN_USING_ENCODER);

        intake_motor_left = hardware_map::get(DcMotorEx, "intake_left");
        intake_motor_left.set_power(0);
        intake_motor_left.set_mode(Dc_motor_ex::Run_mode::RUN_USING_ENCODER);
        intake_motor_left.set_direction(Dc_motor_ex::Direction::REVERSE);
    }

    void activate_intake() {
        intake_motor_right.set_power(-1);
        intake_motor_left.set_power(-1);
    }

    void deactivate_intake() {
        intake_motor_right.set_power(0);
        intake_motor_left.set_power(0);
    }

    void activate_intake_reversed() {
        intake_motor_right.set_power(0.5);
        intake_motor_left.set_power(0.5);
    }

    void set_power(double power) {
        intake_motor_right.set_power(-power);
        intake_motor_left.set_power(-power);
    }
}