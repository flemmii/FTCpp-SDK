#include "hardware/PIDF_coefficients.h"

using namespace std;

std::string PIDF_coefficients::to_string() const {
    return ::to_string(p) + "/" + ::to_string(i) + "/" + ::to_string(d) + "/" + ::to_string(f);
}

PIDF_coefficients::PIDF_coefficients() {
    this->p = this->i = this->d = this->f = 0;
}

PIDF_coefficients::PIDF_coefficients(double p, double i, double d, double f) {
    this->p = p;
    this->i = i;
    this->d = d;
    this->f = f;
}
