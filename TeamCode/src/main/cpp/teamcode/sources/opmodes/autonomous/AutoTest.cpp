#include "opmodes/autonomous/AutoTest.hpp"

#include "opmodes/include_all_header.hpp"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace drive_to_position_controller;

namespace AutoTest
{
    extern "C" void AutoTest()
    {
        // robot::init();
        robot::control_hub = hardware_map::get<Lynx_module>("Control Hub");
        robot::control_hub.set_bulk_caching_mode(Lynx_module::Bulk_caching_mode::MANUAL);
        robot::expansion_hub = hardware_map::get<Lynx_module>("Expansion Hub 2");
        robot::expansion_hub.set_bulk_caching_mode(Lynx_module::Bulk_caching_mode::MANUAL);
        ::gamepads::init();
        drivetrain::init();
        drivetrain::use_centripetal_force = false;
        pinpoint::init();
        drive_to_position_controller::init();
        outtake::elevator::init();
        match_data::current_match::setup_variables_for_match();
        drivetrain::preset_for_auto();

        predefine_sequential_paths({{{0_mm, 0_mm}, {0_mm, 2000_mm}},
                                    {{0_mm, 0_mm}}});

        telemetry::add_line("Initialized");
        telemetry::update();

        wait_for_start();

        if (op_mode_is_active())
        {
            task_scheduler::register_thread([]
                                            {
                       drive_to_position(0, true, 5000ms);
                       sleep(chrono::milliseconds(2000));
                       Position pos = pinpoint::get_position();
                       log(ANDROID_LOG_DEBUG, "dtpc") << "Final Position " << pos << endl;
                       drive_to_position(1, 0_deg, 5000ms);
                       sleep(chrono::milliseconds(2000));
                       pos = pinpoint::get_position();
                       log(ANDROID_LOG_DEBUG, "dtpc") << "Final Position " << pos << endl;
                       linear_op_mode::request_op_mode_stop(); });

            while (!is_stop_requested())
                task_scheduler::execute();
        }

        robot::stop();
    }
}