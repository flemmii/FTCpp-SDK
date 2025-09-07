#include "opmodes/tele_op/Maintenance.hpp"

#include "opmodes/include_all_header.hpp"

using namespace std;

using namespace ::gamepads;

using namespace sdk;
using namespace linear_op_mode;

namespace Maintenance
{
    Position<false, Power, Power> last_power;
    int buttons_pressed_for_switching_centric = 0;
    bool field_centric = true;

    void switch_centric_if_all_buttons_pressed(int buttons_pressed_for_switching_centric)
    {
        if (buttons_pressed_for_switching_centric == 4)
            field_centric = !field_centric;
    }

    extern "C" void Maintenance()
    {
        robot::init();

        if (last_position_storage::is_data_valid() && match_data::current_match::match_number != 0)
            pinpoint::set_position(last_position_storage::get_stored_position());

        gamepad1.sticks.add_update_listener(
            [](float left_stick_x, float left_stick_y, float right_stick_x,
               float right_stick_y)
            {
                outtake::arm::set_power(Power(left_stick_x));
            },
            true, false, false, false);

        gamepad1.a.add_press_listener([]
                                      {

            Millimetre start_extension = intake::extendo::get_extension();
            bool start_extendo_homed = intake::extendo::homed();

            Millimetre start_elevation = outtake::elevator::get_elevation();
            bool start_elevator_homed = outtake::elevator::homed();

            Degree start_arm_rotation = outtake::arm::get_rotation();

            auto extendo_encoder_working = make_shared<bool>(false);
            auto extendo_homing_switch_working = make_shared<bool>(false);

            auto elevator_encoder_working = make_shared<bool>(false);
            auto elevator_homing_switch_working = make_shared<bool>(false);

            auto arm_encoder_working = make_shared<bool>(false);

            constexpr chrono::steady_clock::duration timeout = 5s;
            const chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

            drivetrain::drive<false>({0.5_power, 0_power, 0_power});

            intake::extendo::reset_position();
            outtake::elevator::reset_position();
            outtake::arm::set_power(0.3_power);

            task_scheduler::register_task((function<bool()>)[=] () mutable {
            if (intake::extendo::get_extension() != start_extension)
                *extendo_encoder_working = true;

            if (intake::extendo::homed() != start_extendo_homed)
                *extendo_homing_switch_working = true;

            if (outtake::elevator::get_elevation() != start_elevation)
                *elevator_encoder_working = true;

            if (outtake::elevator::homed() != start_elevator_homed)
                *elevator_homing_switch_working = true;

            if (outtake::arm::get_rotation() != start_arm_rotation)
            {
                *arm_encoder_working = true;
                outtake::arm::set_power(0_power);
            }
            
            return (*extendo_encoder_working && *extendo_homing_switch_working &&
                   *elevator_encoder_working && *elevator_homing_switch_working) || (chrono::steady_clock::now() - start_time > timeout); }, 50ms, true, [=]
                                      {
            telemetry::add_line("Maintenance test results:");

            if (*extendo_encoder_working)
                telemetry::add_data("<span style='color: green;'>Extendo encoder", "working</span>");
            else
                telemetry::add_data("<span style='color: red;'>Extendo encoder", "not working</span>");

            if (*extendo_homing_switch_working)
                telemetry::add_data("<span style='color: green;'>Extendo homing switch", "working</span>");
            else
                telemetry::add_data("<span style='color: red;'>Extendo homing switch", "not working</span>");

            if (*elevator_encoder_working)
            telemetry::add_data("<span style='color: green;'>Elevator encoder", "working</span>");
            else
                telemetry::add_data("<span style='color: red;'>Elevator encoder", "not working</span>");

            if (*elevator_homing_switch_working)
                telemetry::add_data("<span style='color: green;'>Elevator homing switch", "working</span>");
            else
                telemetry::add_data("<span style='color: red;'>Elevator homing switch", "not working</span>");

            if (*arm_encoder_working)
                telemetry::add_data("<span style='color: green;'>Arm encoder", "working</span>");
            else
                telemetry::add_data("<span style='color: red;'>Arm encoder", "not working</span>");

            telemetry::add_data("pinpoint status", pinpoint::pinpoint.device_status_to_string(pinpoint::pinpoint.get_device_status()));

            telemetry::update(); }); });

        gamepad1.x.add_press_listener([]
                                      {
                                        if (intake::extendo::target_extension == 0_mm && outtake::elevator::target_elevation == 0_mm)
                                        {
                                            intake::extendo::set_extension(intake::extendo::MAX_EXTENSION);
                                            outtake::elevator::set_elevation(outtake::elevator::MAX_ELEVATION);
                                        }
                                        else
                                        {
                                            intake::extendo::retract();
                                            outtake::elevator::retract();
                                        }

                                        static int position = 0;
                                        if (position == 0)
                                        {
                                            position = 1;
                                            drive_to_position_controller::drive_to_position({0_mm, -600_mm, 0_deg});
                                        }
                                        else
                                        {
                                            position = 0;
                                            drive_to_position_controller::drive_to_position({0_mm, 0_mm, 0_deg});
                                        }
                                        
                                        Volt lowest_voltage = 20_V;
                                        task_scheduler::register_task((function<bool()>) [lowest_voltage] () mutable {
                                            if (drive_to_position_controller::position_reached || !intake::extendo::moving || !outtake::elevator::moving)
                                                return true;

                                            const Volt current_voltage = Volt(robot::control_hub.get_input_voltage(Voltage_unit::VOLTS));
                                            if (current_voltage < lowest_voltage)
                                                lowest_voltage = current_voltage;

                                            telemetry::add_data("Lowest voltage", to_string(lowest_voltage));
                                            if (lowest_voltage > 9.6_V)
                                                telemetry::add_line("<span style='color: green;'>Florian suggests this battery 🔥👍</span>");
                                            else
                                                telemetry::add_line("<span style='color: red;'>Florian does NOT suggest this battery 💩🙅‍♂️👎</span>");

                                            telemetry::update();
                                            return false;
                                        }, 50ms, true); });

        gamepad1.y.add_press_listener([]
                                      {
            if (outtake::gripper::status != outtake::gripper::Status::OPEN) {
                outtake::gripper::open();
            } else {
                outtake::gripper::close();
            } });

        gamepad1.dpad_up.add_press_listener([]
                                            {
            if (intake::gripper::closed) {
                intake::gripper::open();
            } else {
                intake::gripper::close_losely();
            } });

        gamepad1.dpad_left.add_press_listener([]
                                              {
                                                if (outtake::wrist::manual_set_target_rotation != 0_deg)
                                                    outtake::wrist::set_rotation(0_deg, false, 0_deg);
                                                else
                                                    outtake::wrist::set_rotation(-90_deg, false, 0_deg); });

        gamepad1.dpad_right.add_press_listener([]
                                               {
                                                if (outtake::ready_for_transfer())
                                                    outtake::move_to_high_basket_position();
                                                else
                                                    outtake::move_to_transfer_position(); });

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
            task_scheduler::register_thread([homing_elevator] {
                outtake::elevator::wait_for_movement_finish();
                *homing_elevator = false;
            }); });

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
            task_scheduler::register_thread([homing_extendo] {
                intake::extendo::wait_for_movement_finish();
                *homing_extendo = false;
            }); });

        gamepad2.right_bumper.add_press_listener([]
                                                 {
            if (intake::extendo::target_extension == 0_mm) {
                intake::extendo::set_extension(intake::extendo::MAX_EXTENSION);
            } else {
                intake::extendo::retract();
            } });

        gamepad2.left_bumper.add_press_listener([]
                                                {
            if (outtake::elevator::target_elevation == 0_mm) {
                outtake::elevator::set_elevation(500_mm);
            } else {
                outtake::elevator::retract();
            } });

        gamepad2.a.add_press_listener([]
                                      {
            if (outtake::elevator::target_elevation != outtake::elevator::MAX_ELEVATION)
                outtake::elevator::set_elevation(outtake::elevator::MAX_ELEVATION);
            else
                outtake::elevator::set_elevation(500_mm, true); });

        gamepad2.dpad_down.add_press_listener([]
                                              {
            if (outtake::arm::target_rotation == 0_deg)
                outtake::arm::set_rotation(90_deg);
            else
                outtake::arm::set_rotation(0_deg); });

        gamepad2.dpad_up.add_press_listener([]
                                            {
            if (outtake::arm::linkage::target_extension == 0_mm)
                outtake::arm::linkage::set_length(100_mm);
            else
                outtake::arm::linkage::retract(); });

        drivetrain::preset_for_tele_op();
        drivetrain::use_gyro_support = false;
        drivetrain::use_centripetal_force = false;
        drivetrain::use_controlled_acceleration = false;

        buttons_pressed_for_switching_centric = 0;

        task_scheduler::register_task([]()
                                      { ::gamepads::update(); }, 50ms);

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

        outtake::gripper::open();

        while (!linear_op_mode::is_stop_requested())
            task_scheduler::execute();

        robot::stop();
    }
}