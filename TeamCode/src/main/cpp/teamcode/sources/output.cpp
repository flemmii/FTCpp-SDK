#include "../header/output.h"

using namespace sdk;

namespace output {
    class Servo pixel_servo;

    void init() {
        pixel_servo = hardware_map::get(Servo, "arm_servo");
        pixel_servo.set_position(0.1);
        pixel_servo.get_controller().pwm_disable();
    }

    void release_pixel() {
        pixel_servo.set_position(0.6);
        pixel_servo.get_controller().pwm_enable();
    }

    void retract_servo() {
        pixel_servo.set_position(0.1);
        pixel_servo.get_controller().pwm_enable();
    }

    void deactivate() {
        pixel_servo.get_controller().pwm_disable();
    }
}
