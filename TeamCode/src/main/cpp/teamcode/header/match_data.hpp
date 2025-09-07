#pragma once

namespace match_data
{
    constexpr bool COMPETITION_MODE = false;
    namespace current_match
    {
        extern bool red_alliance;
        extern bool start_right;

        // Autonomous specifications

        // Do not access this before get_match_number() or setup_variables_for_match() was called
        extern int match_number;

        // 0 is no match
        int get_match_number();

        void setup_variables_for_match();
    } // this_match
} // match_data
