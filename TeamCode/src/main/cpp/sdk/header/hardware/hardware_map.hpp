#pragma once

#include <string>

#include "utils.hpp"

namespace sdk::hardware_map
{
    // TODO: This is not complete
    extern jobject hardwareMap;

    jobject get(const jclass &jclazz, const std::string &device_name);

    template <class T>
    jobject get(const std::string &device_name)
    {
        return get(T::jclazz, device_name);
    }
}
// SDK
