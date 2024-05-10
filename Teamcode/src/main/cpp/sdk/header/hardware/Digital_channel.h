//
// Created by fnlg on 10.05.2024.
//

#ifndef FTCROBOTCONTROLLER_DIGITAL_CHANNEL_H
#define FTCROBOTCONTROLLER_DIGITAL_CHANNEL_H

#include <string>

#include "extras/utils.h"

namespace sdk {
    extern jclass DigitalChannel;

    class Digital_channel {
    public:
        jobject digitalChannel;

        enum class Mode {
            INPUT,
            OUTPUT
        };

        Digital_channel() = default;

        Digital_channel(jobject digitalChannel);

        ~Digital_channel();

        [[nodiscard]] Mode get_mode() const;

        void set_mode(Mode mode) const;

        [[nodiscard]] bool get_state() const;

        void set_state(bool state) const;
    };

} // sdk

#endif //FTCROBOTCONTROLLER_DIGITAL_CHANNEL_H
