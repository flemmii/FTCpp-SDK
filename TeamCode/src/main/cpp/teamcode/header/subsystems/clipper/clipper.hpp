#pragma once

namespace clipper
{
    void init();

    void stop();

    bool reset(bool wait_until_done = false);

    bool load_clips(bool wait_until_done = false);

    void wait_for_movement_finish();
}