//
// Created by Frosch PC - V2 on 01.02.2024.
//

#include "../header/elevator.h"

using namespace sdk;

namespace elevator {
    Dc_motor_ex motor_elevator;
    bool encoder_plugged_in = true;

#define ENCODER_STEPS_PER_MM 3.1344 // calibrated; accuracy ~ +-2%
#define MAX_ELEVATION 800

    double target_elevation_in_mm = 0;
    double target_power = 0;

    int layer = 0;

    void init() {
        motor_elevator = hardware_map::get(DcMotorEx, "elevator");
        motor_elevator.set_mode(Dc_motor_ex::Run_mode::STOP_AND_RESET_ENCODER);
        motor_elevator.set_direction(Dc_motor_ex::Direction::REVERSE);
        motor_elevator.set_target_position(0);
        motor_elevator.set_power(0);
        motor_elevator.set_mode(Dc_motor_ex::Run_mode::RUN_TO_POSITION);
    }

    void set_target_power(double power) {
        target_power = power;
        motor_elevator.set_power(target_power);
    }

    void set_power(double power) {
        motor_elevator.set_power(power);
    }

    void set_target_elevation_in_mm(double elevation_in_mm, double power) {
        if (elevation_in_mm < -100 || elevation_in_mm > MAX_ELEVATION || !encoder_plugged_in)
            return;
        target_elevation_in_mm = elevation_in_mm;
        target_power = power;
        motor_elevator.set_target_position((int) (target_elevation_in_mm * ENCODER_STEPS_PER_MM));
        motor_elevator.set_power(target_power);
    }

    void check_encoder() {
        if (motor_elevator.is_busy() && encoder_plugged_in) {

            if (error_detection::detect_fire("elevator", motor_elevator.get_velocity(),
                                             target_power,
                                             3)) {
                encoder_plugged_in = false;
                motor_elevator.set_mode(Dc_motor_ex::Run_mode::RUN_WITHOUT_ENCODER);
            }

        }
    }

    double get_elevation_in_mm() {
        return motor_elevator.get_current_position() / ENCODER_STEPS_PER_MM;
    }

    double get_target_elevation_in_mm() {
        return target_elevation_in_mm;
    }

    double get_max_elevation() {
        return MAX_ELEVATION;
    }

    void stop() {
        motor_elevator.set_power(0);
        motor_elevator.set_motor_disable();
    }

    void set_target_elevation_to_layer(int layer) {
        set_target_elevation_in_mm(76 * layer + 10);
    }

    void set_target_elevation_to_layer() {
        set_target_elevation_in_mm(76 * layer + 10);
    }

    bool is_at_target_position() {
        return (target_elevation_in_mm + 5 > get_elevation_in_mm() &&
                target_elevation_in_mm - 5 < get_elevation_in_mm());
    }

    int get_layer() {
        return layer;
    }

    void set_layer(int layer) {
        if (layer <= 14 && layer >= 0)
            elevator::layer = layer;
    }

    void deactivate() {
        motor_elevator.set_motor_disable();
    }
} // elevator