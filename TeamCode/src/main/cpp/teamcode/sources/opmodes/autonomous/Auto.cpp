#include "opmodes/autonomous/Auto.hpp"

#include "opmodes/include_all_header.hpp"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace drive_to_position_controller;
using namespace outtake;
using namespace intake;

namespace Auto
{
    chrono::steady_clock::time_point start_time;
    Position<> starting_position(0_mm, 0_mm, 0_deg);

    void finish_init_and_wait_for_start()
    {
        pinpoint::set_position(starting_position);

        vector<string> encoder_not_working = error_detection::get_motors_on_fire();
        if (!encoder_not_working.empty())
        {
            for (const string &encoder : encoder_not_working)
                telemetry::add_data("<span style='color: red;'>Encoder not working</span>", encoder);

            telemetry::update();
        }

        deceleration_multiplier = 1;

        telemetry::add_line("Initialized");
        telemetry::update();

        wait_for_start();
        start_time = chrono::steady_clock::now();
    }

    extern "C" void
    Auto()
    {
        if (imageRec_debug_mode_enabled)
        {
            if (match_data::COMPETITION_MODE && match_data::current_match::get_match_number() == 0)
                return;

            robot::init();

            intake::wrist::set_rotation(30_deg, false, 60_deg);
            intake::arm::set_rotation(-60_deg);
            intake::swivel::set_rotation(0_deg);

            /*
            Webcam_name webcam = hardware_map::get<Webcam_name>("Webcam 1");

            Sample_detection *proc = new Sample_detection();

            Vision_portal portal = Vision_portal::Builder().set_camera_resolution(proc->resolution).set_camera(webcam).add_processor(proc).build();
*/

            wait_for_start();

            intake::search_sample({intake::arm::GRIPPING_RADIUS, 0_mm}, 30_deg);

            while (!is_stop_requested())
                task_scheduler::execute();

            // portal.close();

            return;
        }
        robot::init();
        drivetrain::preset_for_auto();

        if (match_data::current_match::start_right)
            specimen::auton();
        else
            sample::auton();

        while (!is_stop_requested())
            task_scheduler::execute();

        sleep(2s);

        last_position_storage::store_position(pinpoint::get_position());
        robot::stop();

        linear_op_mode::request_op_mode_stop();
    }
}