//
// Created by fnlg on 27.01.2024.
//

#include "../header/PID_controller.h"

PID_controller::PID_controller(double kp, double ki, double kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

double PID_controller::calculate_pid_algorithm(double error) {
    integral += error;

    double output = kp * error + ki * integral + kd * (error - previous_error);
    previous_error = error;
    return output;
}

void PID_controller::reset() {
    integral = 0;
    previous_error = 0;
}

void PID_controller::set_pid_gains(double kp, double ki, double kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

double PID_controller::get_kp() const {
    return kp;
}

void PID_controller::set_kp(double kp) {
    this->kp = kp;
}

double PID_controller::get_ki() const {
    return ki;
}

void PID_controller::set_ki(double ki) {
    this->ki = ki;
}

double PID_controller::get_kd() const {
    return kd;
}

void PID_controller::set_kd(double kd) {
    this->kd = kd;
}