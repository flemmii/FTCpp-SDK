//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_PIDF_COEFFICIENTS_H
#define FTCROBOTCONTROLLER_PIDF_COEFFICIENTS_H

#include <string>
#include <jni.h>

namespace sdk {
    struct PIDF_coefficients {
        static jclass jclazz;

        double p;
        double i;
        double d;
        double f;

        [[nodiscard]] std::string to_string() const;

        PIDF_coefficients() : p(0), i(0), d(0), f(0) {};

        PIDF_coefficients(const double &p, const double &i, const double &d, const double &f) : p(
                p), i(i), d(d), f(f) {};
    };
}

#endif //FTCROBOTCONTROLLER_PIDF_COEFFICIENTS_H
