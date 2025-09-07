#pragma once

#include "utils/Position.hpp"

namespace last_position_storage
{
    void store_position(Position<true, Millimetre, Degree> position);

    bool is_data_valid();

    Position<true, Millimetre, Degree> get_stored_position();
}
