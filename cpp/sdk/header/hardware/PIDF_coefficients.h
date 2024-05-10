//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_PIDF_COEFFICIENTS_H
#define FTCROBOTCONTROLLER_PIDF_COEFFICIENTS_H

#include <string>

struct PIDF_coefficients {
    double p;
    double i;
    double d;
    double f;

    [[nodiscard]] std::string to_string() const;

    PIDF_coefficients();

    PIDF_coefficients(double p, double i, double d, double f);
};

#endif //FTCROBOTCONTROLLER_PIDF_COEFFICIENTS_H
