#pragma once

#include <jni.h>

namespace sdk
{
    extern jclass VoltageUnit;
    enum class Voltage_unit
    {
        // TODO: This is not complete
        VOLTS,
        MILLIVOLTS
    };
}
