//
// Created by fnlg on 17.08.2024.
//

#include "utils/PD_controller.hpp"

double PD_controller::calculate_pid_algorithm(const double &error)
{
    double output = kp * error + kd * (error - previous_error);
    previous_error = error;
    return output;
}

void PD_controller::reset()
{
    previous_error = 0;
}