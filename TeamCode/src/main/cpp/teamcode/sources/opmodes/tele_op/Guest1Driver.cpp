#include "opmodes/tele_op/Guest1Driver.hpp"

#include "opmodes/include_all_header.hpp"

using namespace std;

using namespace ::gamepads;

using namespace sdk;
using namespace linear_op_mode;

namespace Guest1Driver
{
    chrono::time_point started_driving = chrono::steady_clock::now();
    constexpr chrono::duration time_allowed = 3min;

    bool is_time_over()
    {
        bool time_over = chrono::steady_clock::now() - started_driving > time_allowed;
        if (time_over)
            drivetrain::drive<false>({0_power, 0_power, 0_power});

        return time_over;
    }

    int buttons_pressed_for_switching_centric = 0;
    bool field_centric = true;

    void switch_centric_if_all_buttons_pressed(int buttons_pressed_for_switching_centric)
    {
        if (buttons_pressed_for_switching_centric == 4)
        {
            field_centric = !field_centric;
        }
    }

    extern "C" void Guest1Driver()
    {
        robot::init();

        drivetrain::preset_for_tele_op();
        drivetrain::use_gyro_support = false;

        gamepad1.sticks.add_update_listener(
            [](float left_stick_x, float left_stick_y, float right_stick_x,
               float right_stick_y)
            {
                if (is_time_over())
                    return;

                if (field_centric)
                    drivetrain::drive(Position<true, Power, Power>{Power(left_stick_x), Power(left_stick_y), Power(right_stick_x)}, 0.5);
                else
                    drivetrain::drive(Position<false, Power, Power>{Power(left_stick_y), Power(left_stick_x), Power(right_stick_x)}, 0.5);
            },
            true, true, true, false);

        gamepad1.left_trigger.add_press_listener(
            []
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);
            });
        gamepad1.left_trigger.add_release_listener(
            []
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
            []()
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);
            });
        gamepad1.left_bumper.add_release_listener(
            []()
            { buttons_pressed_for_switching_centric--; });

        gamepad1.right_bumper.add_press_listener(
            []()
            {
                buttons_pressed_for_switching_centric++;
                switch_centric_if_all_buttons_pressed(
                    buttons_pressed_for_switching_centric);
            });

        gamepad1.right_bumper.add_release_listener(
            []()
            { buttons_pressed_for_switching_centric--; });

        gamepad1.left_trigger.add_press_listener([]
                                                 {
            Degree rotation = pinpoint::get_position().rotation;
            pinpoint::set_rotation(
                    360_deg * (rotation > 0_deg ? floor(rotation.Value() / 360) : ceil(rotation.Value() / 360))); });

        gamepad1.back.add_press_listener([]
                                         {
            outtake::elevator::retract();

            intake::gripper::open();

            started_driving = chrono::steady_clock::now(); });

        buttons_pressed_for_switching_centric = 0;

        task_scheduler::register_task([]()
                                      {
            Position current_position = pinpoint::get_position();
            Vec2d current_speed = pinpoint::get_speed().to_vec();

            telemetry::add_data("buttons pressed for switching centric",
                                buttons_pressed_for_switching_centric);
            telemetry::add_data("position", "x: " + to_string(current_position.x) + " y: " +
                                            to_string(current_position.y) + " rotation: " +
                                            to_string(current_position.rotation));
            telemetry::add_data("speed", "x: " + to_string(current_speed.x) + " y: " +
                                         to_string(current_speed.y));
            telemetry::add_data("time left", to_string(chrono::duration_cast<chrono::seconds>(time_allowed - (chrono::steady_clock::now() - started_driving)).count()) + " s");
            telemetry::update();

            ::gamepads::update(); }, 50ms);

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

        started_driving = chrono::steady_clock::now();

        while (!linear_op_mode::is_stop_requested())
            task_scheduler::execute();

        robot::stop();
    }
}