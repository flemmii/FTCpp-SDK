#pragma once

#include <chrono>

#include "utils/Position.hpp"
#include "field.hpp"

namespace Auto
{
    extern std::chrono::steady_clock::time_point start_time;
    extern Position<> starting_position;
    constexpr Millimetre distance_samples = field::samples::alliance_specific::own_alliance::middle.center().x - field::samples::alliance_specific::own_alliance::field.center().x;

    void finish_init_and_wait_for_start();

    /*
     * This is NOT a comment. It tells the compiler how to create the java files
     * @Autonomous(name = "Auto", preselectTeleOp = "Main")
     */
    extern "C" void Auto();
}