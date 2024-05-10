#include "navigation/Voltage_unit.h"


namespace sdk::voltage_unit {
    jclass VoltageUnit;

    double toVolts(double value, double voltsPerUnit) {
        return value * voltsPerUnit;
    }

    double toMilliVolts(double value, double voltsPerUnit) {
        return value * voltsPerUnit * 1000.0;
    }

    double convert(double value, voltage_unit_type unit, double voltsPerUnit) {
        if (unit == voltage_unit_type::VOLTS) {
            return toVolts(value, voltsPerUnit);
        } else if (unit == voltage_unit_type::MILLIVOLTS) {
            return toMilliVolts(value, voltsPerUnit);
        }
        return 0;
    }
}

