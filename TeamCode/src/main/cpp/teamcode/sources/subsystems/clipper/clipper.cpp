#include "subsystems/clipper/clipper.hpp"

#include "subsystems/clipper/aligner.hpp"
#include "subsystems/clipper/intake.hpp"
#include "Units-master/Units.h"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/intake/extendo.hpp"

namespace clipper
{
    int threads_running = 0;

    void init()
    {
        aligner::init();
        intake::init();
    }

    void stop()
    {
        aligner::stop();
        intake::stop();
    }

    bool reset(bool wait_until_done)
    {
        aligner::set_position(aligner::MAX_POSITION);
        intake::move_to_upper_intake_position();

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [] {}, []
                                                             {
            aligner::wait_for_movement_finish();
            intake::wait_for_movement_finish(); });

        return aligner::right::target_position == aligner::MAX_POSITION && aligner::left::target_position == aligner::MAX_POSITION && intake::target_rotation == intake::UPPER_INTAKE_POSITION;
    }

    bool load_clips(bool wait_until_done)
    {
        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, []
                                                             {
            intake::move_to_deposit_position(true);
            sleep(200ms);
            aligner::setup_for_clips(7, true);
            intake::move_to_upper_intake_position(true);
            ::intake::extendo::retract(); }, aligner::wait_for_movement_finish);

        return aligner::right::target_position == 3.5_in && aligner::left::target_position == 3.5_in && intake::target_rotation == intake::UPPER_INTAKE_POSITION && ::intake::extendo::target_extension == 0_mm;
    }

    void wait_for_movement_finish()
    {
        while (intake::moving || aligner::right::moving || aligner::left::moving || threads_running != 0)
        {
            intake::wait_for_movement_finish();
            aligner::wait_for_movement_finish();
        }
    }
}