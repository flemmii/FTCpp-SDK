#pragma once

#include <string>

#include "utils.hpp"

namespace sdk
{
    class Analog_input
    {
    public:
        static jclass jclazz;
        jobject analogInput{};

        Analog_input() = default;

        Analog_input(const jobject &analogInput);

        ~Analog_input();

        Analog_input &operator=(const Analog_input &analog_input);

        virtual Analog_input &operator=(const jobject &analogInput);

        [[nodiscard]] double get_voltage() const;

        [[nodiscard]] double get_max_voltage() const;

        [[nodiscard]] std::string get_device_name() const;

        [[nodiscard]] std::string get_connection_info() const;

        [[nodiscard]] int get_version() const;

        void reset_device_configuration_for_op_mode() const;

        void close() const;
    };

} // sdk
