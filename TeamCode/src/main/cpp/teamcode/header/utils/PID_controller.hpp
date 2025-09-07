#pragma once

class PID_controller
{
    /*
     * PID Controller
     * Based on https://en.wikipedia.org/wiki/PID_controller
     */

public:
    // Proportional gain
    double kp;
    // Integral gain
    double ki;
    // Derivative gain
    double kd;

    double integral = 0;
    double previous_error = 0;

    PID_controller(const double &kp, const double &ki, const double &kd) : kp(kp), ki(ki), kd(kd) {}

    double calculate_pid_algorithm(const double &error);

    void reset();
};
