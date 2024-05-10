#ifndef FTC_FROG_VOLTAGEUNIT_H
#define FTC_FROG_VOLTAGEUNIT_H

#include <jni.h>

namespace sdk::voltage_unit {
    extern jclass VoltageUnit;
    enum class voltage_unit_type {
        VOLTS,
        MILLIVOLTS
    };

    double toVolts(double value, double voltsPerUnit);

    double toMilliVolts(double value, double voltsPerUnit);

    double convert(double value, voltage_unit_type unit, double voltsPerUnit);
}


#endif
