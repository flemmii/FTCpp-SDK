#ifndef FTC_FROG_VOLTAGEUNIT_H
#define FTC_FROG_VOLTAGEUNIT_H

#include <jni.h>

namespace sdk {
    extern jclass VoltageUnit;
    enum class Voltage_unit {
        // TODO: This is not complete
        VOLTS,
        MILLIVOLTS
    };
}


#endif
