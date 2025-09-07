#include "hardware/PIDF_coefficients.hpp"

using namespace std;

namespace sdk
{
    jclass PIDF_coefficients::jclazz;

    std::string PIDF_coefficients::to_string() const
    {
        return ::to_string(p) + "/" + ::to_string(i) + "/" + ::to_string(d) + "/" + ::to_string(f);
    }
}