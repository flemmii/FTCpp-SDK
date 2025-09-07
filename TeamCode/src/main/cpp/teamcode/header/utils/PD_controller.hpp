#pragma once

class PD_controller
{
public:
    // Proportional gain
    double kp;
    // Derivative gain
    double kd;

    double previous_error = 0;

    PD_controller(const double &kp, const double &kd) : kp(kp), kd(kd) {}

    double calculate_pid_algorithm(const double &error);

    void reset();
};
