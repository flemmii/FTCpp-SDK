#include "opmodes/tele_op/Main.hpp"

#include "opmodes/include_all_header.hpp"

using namespace std;

using namespace ::gamepads;

using namespace sdk;
using namespace linear_op_mode;

namespace Main
{
    int buttons_pressed_for_switching_centric = 0;
    bool field_centric = true;
    int threads_running = 0;

    void switch_centric_if_all_buttons_pressed(int buttons_pressed_for_switching_centric)
    {
        if (buttons_pressed_for_switching_centric == 4)
            field_centric = !field_centric;
    }

    void sample_cycle()
    {
        if (outtake::elevator::target_elevation != 0_mm)
        {
            threads_running++;
            task_scheduler::register_thread([]
                                            {
                        
                        if (outtake::gripper::open(true))
                            outtake::move_to_transfer_position();
                        threads_running--; });
        }
        else
        {
            threads_running++;
            task_scheduler::register_thread([]
                                            {
                            outtake::move_to_high_basket_position();
                        threads_running--; });
        }
    }

    bool try_gripping = false;
    void gripper()
    {
        if (outtake::gripper::status != outtake::gripper::Status::OPEN)
        {
            outtake::gripper::open();
        }
        else if (robot::ready_for_transfer())
        {
            outtake::gripper::close();
        }
        else if (intake::gripper::closed)
        {
            intake::gripper::open();
        }
        else
        {
            try_gripping = true;
            threads_running++;
            task_scheduler::register_thread([]
                                            {
                    bool got_sample = false;
                    while (try_gripping && !got_sample)
                        got_sample = intake::grip_sample(true);
                    
                    if (got_sample && robot::scoring_samples)
                        robot::transfer();
                    else if (got_sample && !robot::scoring_samples)
                        intake::set_position({intake::arm::GRIPPING_RADIUS, 0_mm}, -90_deg, 0_deg, 0_deg);
                
                threads_running--; });
        }
    }

    bool scored_specimen = false;
    void specimen_cycle()
    {
        if (outtake::arm::target_rotation < -70_deg)
            outtake::get_specimen_and_move_to_high_chamber_position();
        else
            outtake::move_to_specimen_intake_position();
    }

    void retract_intake()
    {
        if (robot::scoring_samples)
            robot::transfer();
        else
            intake::set_position({intake::arm::GRIPPING_RADIUS, 0_mm}, -90_deg, 0_deg, 0_deg);
    }

    extern "C" void Main()
    {
        const bool match_number_zero = match_data::current_match::get_match_number() == 0;
        if (match_data::COMPETITION_MODE && match_number_zero)
            return;
        robot::init(match_number_zero);

        // if (last_position_storage::is_data_valid() && match_data::current_match::match_number != 0)
        // pinpoint::set_position(last_position_storage::get_stored_position());

        // A-DRIVER
        gamepad1.sticks.add_update_listener(
            [](float left_stick_x, float left_stick_y,
               float right_stick_x,
               float right_stick_y)
            {
                float forwards = (3.125 * left_stick_y * left_stick_y - 4.375 * fabs(left_stick_y) + 2.25) * left_stick_y;
                float sidewards = (3.125 * left_stick_x * left_stick_x - 4.375 * fabs(left_stick_x) + 2.25) * left_stick_x;
                float turn = (3.125 * right_stick_x * right_stick_x - 4.375 * fabs(right_stick_x) + 2.25) * right_stick_x;

                if (field_centric)
                    drivetrain::drive(Position<true, Power, Power>{Power(sidewards), Power(forwards), Power(turn)});
                else
                    drivetrain::drive(Position<false, Power, Power>{Power(forwards), Power(sidewards), Power(turn)});
            },
            true, true, true, false);

        gamepad1.left_stick_button.add_press_listener(
            []()
            {
                Degree rotation = pinpoint::get_position().rotation;
                pinpoint::set_rotation(
                    360_deg * (rotation > 0_deg ? floor(rotation.Value() / 360) : ceil(rotation.Value() / 360)));
            });

        gamepad1.left_trigger.add_press_listener(
            []
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);

                specimen_cycle();
            });
        gamepad1.left_trigger.add_release_listener([]
                                                   { buttons_pressed_for_switching_centric--; });

        gamepad1.right_trigger.add_press_listener(
            []
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);
            });
        gamepad1.right_trigger.add_release_listener(
            []
            { buttons_pressed_for_switching_centric--; });

        gamepad1.left_bumper.add_press_listener(
            []
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);

                sample_cycle();
            });

        gamepad1.left_bumper.add_release_listener(
            []
            { buttons_pressed_for_switching_centric--; });

        gamepad1.right_bumper.add_press_listener(
            []
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);
            });
        gamepad1.right_bumper.add_release_listener(
            []
            { buttons_pressed_for_switching_centric--; });

        gamepad1.right_trigger.add_press_listener([]
                                                  {
            if (intake::arm::target_rotation == intake::arm::TRANSFER_ROTATION)
                intake::move_to_hovering_position({intake::arm::GRIPPING_RADIUS, 0_mm});
            else
                retract_intake(); });

        gamepad1.right_bumper.add_press_listener([]
                                                 {
            if (intake::extendo::target_extension <= intake::TRANSFER_EXTENSION)
                intake::move_to_hovering_position({intake::extendo::MAX_EXTENSION, 0_mm});
            else
                retract_intake(); });

        gamepad1.a.add_press_listener([]
                                      {
            if (outtake::elevator::target_elevation != outtake::elevator::MAX_ELEVATION)
                outtake::elevator::set_elevation(outtake::elevator::MAX_ELEVATION);
            else
                outtake::elevator::set_elevation(500_mm, false, true); });

        gamepad1.b.add_press_listener(gripper);

        gamepad1.b.add_release_listener([]
                                        { try_gripping = false; });

        gamepad1.y.add_press_listener(
            []
            { drivetrain::use_gyro_support = !drivetrain::use_gyro_support; });

        // ----------------------------
        // B-Driver
        // ----------------------------

        shared_ptr<bool> homing_elevator = make_shared<bool>(false);
        gamepad2.sticks.add_update_listener(
            [homing_elevator](float left_stick_x, float left_stick_y, float right_stick_x,
                              float right_stick_y)
            {
                if (!*homing_elevator)
                    outtake::elevator::set_power(Power(left_stick_y));
            },
            false, true, false, false);

        gamepad2.left_stick_button.add_press_listener([homing_elevator]
                                                      {
        *homing_elevator = true;
        outtake::elevator::reset_position();
        threads_running++;
        thread([homing_elevator] {
            outtake::elevator::wait_for_movement_finish();
            *homing_elevator = false;
            threads_running--;
        }).detach(); });

        shared_ptr<bool> homing_extendo = make_shared<bool>(false);
        gamepad2.sticks.add_update_listener(
            [homing_extendo](float left_stick_x, float left_stick_y, float right_stick_x,
                             float right_stick_y)
            {
                if (!*homing_extendo)
                    intake::extendo::set_power(Power(right_stick_y));
            },
            false, false, false, true);

        gamepad2.right_stick_button.add_press_listener([homing_extendo]
                                                       {
            *homing_extendo = true;
            intake::extendo::reset_position();
            threads_running++;
            task_scheduler::register_thread([homing_extendo]
                                            {
                intake::extendo::wait_for_movement_finish();
                *homing_extendo = false;
                threads_running--; }); });

        gamepad2.right_trigger.add_press_listener([]
                                                  {
            if (intake::arm::target_rotation == intake::arm::TRANSFER_ROTATION)
                intake::move_to_hovering_position({intake::arm::GRIPPING_RADIUS, 0_mm});
            else
                retract_intake(); });

        gamepad2.right_bumper.add_press_listener([]
                                                 {
            if (intake::extendo::target_extension <= intake::TRANSFER_EXTENSION)
                intake::move_to_hovering_position({intake::extendo::MAX_EXTENSION, 0_mm});
            else
                retract_intake(); });

        gamepad2.left_trigger.add_press_listener(specimen_cycle);
        gamepad2.left_bumper.add_press_listener(sample_cycle);

        gamepad2.a.add_press_listener([]
                                      {
            if (outtake::elevator::target_elevation != outtake::elevator::MAX_ELEVATION)
                outtake::elevator::set_elevation(outtake::elevator::MAX_ELEVATION);
            else
                outtake::elevator::set_elevation(500_mm, false, true); });

        gamepad2.b.add_press_listener(gripper);

        gamepad2.b.add_release_listener([]
                                        { try_gripping = false; });

        gamepad2.x.add_press_listener([]
                                      { robot::scoring_samples = !robot::scoring_samples; });

        gamepad2.y.add_press_listener([]
                                      {
            static bool started_ascend = true;

            started_ascend = !started_ascend;
            if (!started_ascend)
            {
                intake::arm::set_rotation(-90_deg);
                intake::swivel::set_rotation(0_deg);
                outtake::arm::set_rotation(0_deg);
                outtake::elevator::set_elevation(outtake::elevator::MAX_ELEVATION - 200_mm);
                intake::extendo::set_extension(intake::extendo::MAX_EXTENSION);
            }
            else
            {
                threads_running++;
                task_scheduler::register_thread([]
                                                {
                                                intake::extendo::set_extension(250_mm, false, true);
                                                    sleep(1s);
                                                    outtake::elevator::set_elevation(595_mm, true);
                                                    outtake::arm::wait_for_movement_finish();
                                                    outtake::elevator::wait_for_movement_finish();
                                                    if (outtake::arm::target_rotation == 0_deg && outtake::elevator::target_elevation == outtake::elevator::MAX_ELEVATION)
                                                        outtake::elevator::set_elevation(50_mm, false, true);

                                                threads_running--; });
                                        } });

        gamepad2.dpad_left.add_press_listener([]
                                              {
            if (!outtake::ready_for_transfer())
            {
                threads_running++;
                task_scheduler::register_thread([]
                                                {
                        if (outtake::gripper::open(true))
                            outtake::move_to_transfer_position();
                        threads_running--; });
            }
            else
            {
                outtake::move_to_low_basket_position();
            } });

        gamepad2.dpad_right.add_press_listener([]
                                               {
            if (intake::gripper::swivel::manual_set_target_rotation == 0_deg)
                {
                    intake::gripper::swivel::set_rotation(90_deg);
                }
                else
                {
                    intake::gripper::swivel::set_rotation(0_deg);
                } });

        drivetrain::preset_for_tele_op();
        drivetrain::use_gyro_support = false;
        drivetrain::use_centripetal_force = false;
        drivetrain::use_controlled_acceleration = false;

        buttons_pressed_for_switching_centric = 0;

        task_scheduler::register_task([]()
                                      {
            Position current_position = pinpoint::get_position();
            Position current_speed = pinpoint::get_speed();

            telemetry::add_data("position", "x: " + to_string(current_position.x) + " y: " +
                                                to_string(current_position.y) + " rotation: " +
                                                to_string(current_position.rotation));
            telemetry::add_data("speed", "x: " + to_string(current_speed.x) + " y: " +
                                             to_string(current_speed.y) + " rotation: " +
                                             to_string(current_speed.rotation));
            telemetry::add_data("elevation", to_string(outtake::elevator::get_elevation()));
            telemetry::add_data("elevation speed", to_string(outtake::elevator::get_elevation_speed()));
            telemetry::add_data("extension", to_string(intake::extendo::get_extension()));
            telemetry::add_data("extension speed", to_string(intake::extendo::get_extension_speed()));
            telemetry::add_data("rotation", to_string(outtake::arm::get_rotation()));
            telemetry::add_data("rotation speed", to_string(outtake::arm::get_rotation_speed()));
            telemetry::add_data("scoring samples", robot::scoring_samples);

        /*
        intake::gripper::Sample_color color = intake::gripper::get_sample_color();
        if (color == intake::gripper::Sample_color::NONE)
            log(ANDROID_LOG_DEBUG, "DEBUG") << "color: NONE" << endl;
        else if (color == intake::gripper::Sample_color::RED)
            log(ANDROID_LOG_DEBUG, "DEBUG") << "color: RED" << endl;
        else if (color == intake::gripper::Sample_color::BLUE)
            log(ANDROID_LOG_DEBUG, "DEBUG") << "color: BLUE" << endl;
        else if (color == intake::gripper::Sample_color::YELLOW)
            log(ANDROID_LOG_DEBUG, "DEBUG") << "color: YELLOW" << endl;
            */

        ::gamepads::update();

        telemetry::update(); }, 50ms);

        if (op_mode_is_active())
            telemetry::speak(
                "Lass do mol d'Fenger weg vom ControlHub bis ser initialisiert isch",
                "swg", "DE");

        telemetry::add_line("Initialized");
        telemetry::update();

        vector<string> encoder_not_working = error_detection::get_motors_on_fire();

        if (!encoder_not_working.empty())
        {
            for (const string &encoder : encoder_not_working)
                telemetry::add_data("<span style='color: red;'>Encoder not working</span>", encoder);

            telemetry::update();
        }

        wait_for_start();

        if (match_data::current_match::match_number != 0)
        {
            outtake::reset();
            intake::reset();
        }
        else
        {
            outtake::gripper::open();
        }

        while (!linear_op_mode::is_stop_requested())
            task_scheduler::execute();

        robot::stop();

        while (threads_running > 0)
            ;
    }
}