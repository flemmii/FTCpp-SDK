#include "../header/suspension_arm_and_drone_launcher.h"

using namespace sdk;

namespace suspension_arm_and_drone_launcher {
    sdk::Dc_motor_ex motor_suspension_arm;

    class sdk::Servo drone_launcher_servo;

    bool encoder_plugged_in = true;

    double ENCODER_STEPS_PER_DEGREE = 265;
    double MAX_ELEVATION = 100;
    double target_elevation_in_degrees = 0;
    double target_power = 0;

    void init() {
        motor_suspension_arm = hardware_map::get(DcMotorEx, "suspension_arm");
        motor_suspension_arm.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
        motor_suspension_arm.set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
        motor_suspension_arm.set_direction(Dc_motor_ex::Direction::REVERSE);
        motor_suspension_arm.set_target_position(0);
        motor_suspension_arm.set_power(0);
        motor_suspension_arm.set_mode(Dc_motor_ex::Run_mode::RUN_TO_POSITION);

        drone_launcher_servo = hardware_map::get(Servo, "drone_launcher_servo");
        drone_launcher_servo.get_controller().pwm_disable();
        drone_launcher_servo.set_position(0);
    }

    void set_target_power(double power) {
        target_power = power;
        motor_suspension_arm.set_power(power);
    }

    void set_target_elevation_in_degrees(double elevation_in_degrees,
                                         double power) {
        if (elevation_in_degrees < 0 || elevation_in_degrees > MAX_ELEVATION || !encoder_plugged_in)
            return;
        target_elevation_in_degrees = elevation_in_degrees;
        target_power = power;
        motor_suspension_arm.set_target_position(
                (int) (target_elevation_in_degrees * ENCODER_STEPS_PER_DEGREE));
        motor_suspension_arm.set_power(target_power);
    }

    void check_encoder() {
        if (motor_suspension_arm.is_busy() && encoder_plugged_in) {
            if (error_detection::detect_fire("suspension_arm", motor_suspension_arm.get_velocity(),
                                             target_power, 3)) {
                encoder_plugged_in = false;
                motor_suspension_arm.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
            }
        }
    }

    void use_encoder(bool useEncoder) {
        encoder_plugged_in = useEncoder;
        if (useEncoder) {
            motor_suspension_arm.set_mode(Dc_motor_ex::Run_mode::RUN_TO_POSITION);
        } else {
            motor_suspension_arm.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
        }
    }

    double get_elevation_in_degrees() {
        return motor_suspension_arm.get_current_position() / ENCODER_STEPS_PER_DEGREE;
    }

    double get_target_elevation_in_degrees() {
        return target_elevation_in_degrees;
    }

    double get_max_elevation() {
        return MAX_ELEVATION;
    }

    void stop() {
        motor_suspension_arm.set_power(0);
    }

    bool is_at_target_position() {
        return (get_target_elevation_in_degrees() + 1 > get_elevation_in_degrees() &&
                get_target_elevation_in_degrees() - 1 < get_elevation_in_degrees());
    }

    void launch_drone() {
        drone_launcher_servo.set_position(1);
        drone_launcher_servo.get_controller().pwm_enable();
    }

    void test() {
        drone_launcher_servo.get_controller().pwm_disable();
    }
}
