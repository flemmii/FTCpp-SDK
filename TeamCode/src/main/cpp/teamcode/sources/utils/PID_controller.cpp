//
// Created by fnlg on 27.01.2024.
//

#include "utils/PID_controller.hpp"

double PID_controller::calculate_pid_algorithm(const double &error)
{
    integral += error;

    double output = kp * error + ki * integral + kd * (error - previous_error);
    previous_error = error;
    return output;
}

void PID_controller::reset()
{
    integral = 0;
    previous_error = 0;
}