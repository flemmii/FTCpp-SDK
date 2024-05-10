//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_PID_CONTROLLER_H
#define FTCROBOTCONTROLLER_PID_CONTROLLER_H

class PID_controller {
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

    PID_controller(double kp, double ki, double kd);

    double calculate_pid_algorithm(double error);

    void reset();

    void set_pid_gains(double kp, double ki, double kd);

    [[nodiscard]] double get_kp() const;

    void set_kp(double kp);

    [[nodiscard]] double get_ki() const;

    void set_ki(double ki);

    [[nodiscard]] double get_kd() const;

    void set_kd(double kd);
};


#endif //FTCROBOTCONTROLLER_PID_CONTROLLER_H
