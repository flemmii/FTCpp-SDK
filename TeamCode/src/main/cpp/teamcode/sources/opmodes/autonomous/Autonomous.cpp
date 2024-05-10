#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"

#include "../../../header/opmodes/autonomous/Autonomous.h"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace drive_to_position_controller;

extern "C"
JNIEXPORT void JNICALL
Java_org_firstinspires_ftc_teamcode_opmodes_autonomous_AutonomousAllmaechtig_opMode(JNIEnv *env,
                                                                                    jobject thiz) {
    /*
    init_sdk
    robot::init();

    Position starting_position;
    double target_x_team_prop_far_from_backdrop = 500;
    double target_x_team_prop_close_to_backdrop = 1700;
    if (start_close_to_backdrop && red_alliance) {
        starting_position = {1760, -90, 0}; //Close to backdrop & red alliance
    } else if (!start_close_to_backdrop && red_alliance) {
        starting_position = {560, -90, 0}; //Far from Backdrop & red alliance
    } else if (start_close_to_backdrop && !red_alliance) {
        starting_position = {-1650, 0, 0}; //Close to backdrop & blue alliance
    } else {
        starting_position = {-450, 0, 0}; //Far from Backdrop & blue alliance
    }
    track_position::set_position(starting_position);

    auto start_time = chrono::high_resolution_clock::now();

    set_max_deviation(50, 50, 10);
    set_max_motor_power(1);

    last_position_storage::init(true);
    drivetrain::use_gyro_support(false);
    drivetrain::use_dead_wheel_support(false);
    drivetrain::use_drive_motor_encoder_support(false);
    drivetrain::use_headless(false);
    drivetrain::set_ms_from_zero_to_max_speed(0);

    thread([] {
        while (!is_stop_requested()) {
            Position current_position = track_position::get_position();
            Position target_position = get_target_position();
            telemetry::add_data("loop time tracking", track_position::get_loop_time());
            telemetry::add_data("current position", "x: " + to_string(current_position.x) + " y: " +
                                                    to_string(current_position.y) + " rotation: " +
                                                    to_string(current_position.rotation));
            telemetry::add_data("target position", "x: " + to_string(target_position.x) + " y: " +
                                                   to_string(target_position.y) + " rotation: " +
                                                   to_string(target_position.rotation));
            telemetry::update();
        }
    }).detach();

    telemetry::add_line("Initialized");
    telemetry::update();
    wait_for_start();
    auto start_time_of_auto = chrono::high_resolution_clock::now();

    drive_to_position_controller::activate();
    // unsigned int pixels_in_robot = 2;
    if (op_mode_is_active()) {
        jclass Camera = env->FindClass("org/firstinspires/ftc/teamcode/camera/Camera");
        int position_team_prop = static_cast<int>(env->CallIntMethod(
                env->GetStaticObjectField(Camera, env->GetStaticFieldID(Camera, "teamPropDetection",
                                                                        "Lorg/firstinspires/ftc/teamcode/camera/TeamPropDetection;")),
                env->GetMethodID(
                        env->FindClass("org/firstinspires/ftc/teamcode/camera/TeamPropDetection"),
                        "getPositionTeamProp", "()I")));
        env->DeleteLocalRef(Camera);

        if (!red_alliance) {
            position_team_prop = 2 - position_team_prop;
        }
        telemetry::add_data("position team prop", position_team_prop);
        telemetry::update();

        double target_y_team_prop = 800;
        if (start_close_to_backdrop) {
            if (position_team_prop == 0) {
                set_target_position(target_x_team_prop_close_to_backdrop, target_y_team_prop, 0,
                                    4000);
                set_max_deviation(20, 20, 5);
                set_target_position(target_x_team_prop_close_to_backdrop, target_y_team_prop - 100, 90,
                                    2000);
            } else if (position_team_prop == 1) {
                //set_target_position(target_x_team_prop_close_to_backdrop, target_y_team_prop - 100,
                //                    0, 4000);
                set_max_deviation(20, 20, 5);
                set_target_position(target_x_team_prop_close_to_backdrop + 470,
                                    target_y_team_prop + 300,
                                    90, 2000);
            } else {
                set_max_deviation(20, 20, 5);
                set_target_position(target_x_team_prop_close_to_backdrop + 570,
                                    target_y_team_prop - 50,
                                    90, 4000);
            }
        } else {
            set_target_position(450, 300, 0, 2000);
            set_target_position(numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN(), 180, 2000);
            if (position_team_prop == 0) {
                set_max_deviation(20, 20, 5);
                set_target_position(target_x_team_prop_far_from_backdrop - 80, target_y_team_prop - 230, 135,
                                    2000);
            } else if (position_team_prop == 1) {
                set_max_deviation(20, 20, 5);
                set_target_position(target_x_team_prop_far_from_backdrop, target_y_team_prop,
                                    180, 2000);
            } else {
                set_max_deviation(20, 20, 5);
                set_target_position(target_x_team_prop_far_from_backdrop + 150,
                                    target_y_team_prop - 300, 225, 2000);
            }
        }
        set_max_deviation(50, 50, 10);
        deactivate();
        intake::set_power(-0.3);


        start_time = chrono::high_resolution_clock::now();
        while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < 500);
        intake::deactivate_intake();
        //pixels_in_robot--;
        activate();
        if (!start_close_to_backdrop && position_team_prop != 1) {
            set_target_position(450, 300, 180, 2000);
        }

        double target_y = 620;
        if (position_team_prop == 2) {
            target_y -= 152;
        } else if (position_team_prop == 0) {
            target_y += 152;
        }


        //if ((score_pixel_at_left_backdrop_position && red_alliance) || (!score_pixel_at_left_backdrop_position && !red_alliance)) {
        //target_y += 76;
        //}

        if (!start_close_to_backdrop) {
            set_max_deviation(20, 20, 5);
            set_target_position(target_x_team_prop_far_from_backdrop, 100, 180, 2000);
            set_target_position(target_x_team_prop_far_from_backdrop, 100, 90, 2000);
            set_target_position(2400, 0, 90, 3000);
            deactivate();

            if (frox) {
                start_time = chrono::high_resolution_clock::now();
                while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                        chrono::high_resolution_clock::now() - start_time).count() < 5000);
            }
            activate();
            set_max_deviation(50, 50, 10);
        }

        elevator::set_target_elevation_in_mm(170);
        set_target_position(2400, target_y, 90, 3000);
        set_max_deviation(20, 20, 5);
        set_target_position(2750, target_y, 90, 4000);
        deactivate();
        set_max_deviation(50, 50, 10);
        drivetrain::drive_without_any_enhancements(0.3, 0, 0);
        drivetrain::do_drive();
        start_time = chrono::high_resolution_clock::now();
        //TODO: This sleep is stupid. Make it great again
        while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < 500);
        drivetrain::stop();

        output::release_pixel();
        start_time = chrono::high_resolution_clock::now();
        while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < 500);
        drivetrain::drive_without_any_enhancements(-0.3, 0, 0);
        drivetrain::do_drive();
        start_time = chrono::high_resolution_clock::now();
        while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < 200);
        drivetrain::stop();
        elevator::set_target_elevation_in_mm(0);
        drive_to_position_controller::set_target_position(numeric_limits<double>::quiet_NaN(),
                                                          numeric_limits<double>::quiet_NaN(),
                                                          numeric_limits<double>::quiet_NaN());
        drive_to_position_controller::activate();
        if (start_close_to_backdrop && !alliance_partner_is_scoring_at_backdrop) {
            //set_target_position(2500, target_y, 90, 2000);
            set_max_deviation(50, 50, 5);
            set_target_position(2600, 750, 150, 2000);
            elevator::deactivate();
            //set_target_position(2000, 1250, 135, 2000);
            set_target_position(2200, 1230, 90, 2000);
            output::retract_servo();
            intake::activate_intake();
            set_max_deviation(50, 20, 5);
            set_target_position(-220, 1230, 90, 4000);
            output::deactivate();

            start_time = chrono::high_resolution_clock::now();
            while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time).count() < 500);
            set_target_position(0, 1230, 90, 2000);
            set_target_position(-220, 1230, 90, 2000);
            set_max_deviation(50, 50, 5);

            start_time = chrono::high_resolution_clock::now();
            while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time).count() < 500);
            intake::deactivate_intake();
            set_target_position(2400, 1230, 90, 4000);
            set_target_position(numeric_limits<double>::quiet_NaN(),
                                numeric_limits<double>::quiet_NaN(), -65, 2000);
            set_target_position(2700, 1000, -65, 2000);
            intake::activate_intake_reversed();
            set_target_position(2400, 1230, -65, 4000);
            intake::deactivate_intake();

            // Second cycle
            if (chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time_of_auto).count() < 20500) {
                set_target_position(numeric_limits<double>::quiet_NaN(),
                                    numeric_limits<double>::quiet_NaN(), 100, 3000);
                intake::activate_intake();
                set_max_deviation(50, 20, 5);
                set_target_position(-220, 1180, 90, 4000);

                start_time = chrono::high_resolution_clock::now();
                while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                        chrono::high_resolution_clock::now() - start_time).count() < 500);
                set_target_position(0, 1180, 90, 1000);
                set_target_position(-220, 1180, 90, 1000);
                set_max_deviation(50, 50, 5);

                start_time = chrono::high_resolution_clock::now();
                while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                        chrono::high_resolution_clock::now() - start_time).count() < 500);
                intake::deactivate_intake();
                set_target_position(2400, 1180, 90, 4000);
                set_target_position(numeric_limits<double>::quiet_NaN(),
                                    numeric_limits<double>::quiet_NaN(), -65, 1000);
                set_target_position(2700, 1000, -65, 1000);
                intake::set_power(-0.6);
                set_target_position(2400, 1180, -65, 4000);
                intake::deactivate_intake();
            }

            set_target_position(2600, 1100, -65, 2000);
        } else if (start_close_to_backdrop) {
            set_target_position(2500, target_y, 90, 8000);
            set_target_position(2500, 100, 90, 2000);
            elevator::deactivate();
            output::deactivate();
        }
        // Collect pixel of alliance partner
        if (!start_close_to_backdrop && !alliance_partner_is_scoring_on_spike_mark) {
            set_target_position(2600, -20, 90, 2000);
            set_target_position(2100, -20, 90, 2000);
            intake::set_power(0.6);
            drivetrain::drive_without_any_enhancements(0.3, 0, 0);
            drivetrain::do_drive();
            start_time = chrono::high_resolution_clock::now();
            while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time).count() < 1000);
            intake::deactivate_intake();
            drivetrain::stop();

            set_max_deviation(20, 20, 5);
            if (position_team_prop == 0) {
                set_target_position(2300, target_y_team_prop - 100, 90, 2000);
                set_target_position(target_x_team_prop_close_to_backdrop - 400,
                                    target_y_team_prop - 100, 90, 2000);
                intake::set_power(-0.3);
                set_target_position(2600, target_y_team_prop - 100, 90, 2000);
                intake::deactivate_intake();
            } else if (position_team_prop == 1) {
                set_target_position(2300, target_y_team_prop + 50, 90, 2000);
                set_target_position(target_x_team_prop_close_to_backdrop - 160,
                                    target_y_team_prop + 50, 90, 2000);
                intake::set_power(-0.3);
                set_target_position(2600, target_y_team_prop + 50, 90, 2000);
                intake::deactivate_intake();
            } else {
                set_target_position(2300, target_y_team_prop, 90, 2000);
                set_target_position(target_x_team_prop_close_to_backdrop + 200, target_y_team_prop,
                                    90, 2000);
                intake::set_power(-0.3);
                set_target_position(2600, target_y_team_prop, 90, 2000);
                intake::deactivate_intake();
            }
            set_max_deviation(50, 50, 5);

            set_target_position(2650, target_y, 90, 2000);
        } else if (!start_close_to_backdrop && !frox) {
            output::retract_servo();
            start_time = chrono::high_resolution_clock::now();
            while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                    chrono::high_resolution_clock::now() - start_time).count() < 3000);
        }

        if (frox) {
            output::retract_servo();
            set_target_position(2600, -10, 90, 6000);
            set_target_position(2600, -10, 0, 6000);
        }

        start_time = chrono::high_resolution_clock::now();
        while (!is_stop_requested() && chrono::duration_cast<chrono::milliseconds>(
                chrono::high_resolution_clock::now() - start_time).count() < 1000);

        //TODO:Last Position is not calibrated
        last_position_storage::store_data(track_position::get_position(),
                                          track_position::get_extra_degrees());
        deactivate();
        drivetrain::stop(); //Brake all motors to minimize positional error of last_position_storage
        stop();
    }
     */
}
#pragma clang diagnostic pop