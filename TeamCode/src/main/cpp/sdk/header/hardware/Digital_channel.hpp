#pragma once

#include <string>

#include "utils.hpp"

namespace sdk
{
    class Digital_channel
    {
    public:
        static jclass jclazz;
        static jclass jclazz_Mode;
        jobject digitalChannel;

        enum class Mode
        {
            INPUT,
            OUTPUT
        };

        Digital_channel() = default;

        Digital_channel(const jobject &digitalChannel);

        ~Digital_channel();

        Digital_channel &operator=(const Digital_channel &digital_channel);

        Digital_channel &operator=(const jobject &digitalChannel);

        [[nodiscard]] Mode get_mode() const;

        void set_mode(const Mode &mode) const;

        [[nodiscard]] bool get_state() const;

        void set_state(const bool &state) const;
    };

} // sdk
