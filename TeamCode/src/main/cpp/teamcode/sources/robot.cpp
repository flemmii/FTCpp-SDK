#include "robot.hpp"

#include <semaphore.h>

#include "match_data.hpp"
#include "subsystems/driving/drivetrain.hpp"
#include "subsystems/outtake/outtake.hpp"
#include "subsystems/intake/intake.hpp"
#include "subsystems/driving/pinpoint.hpp"
#include "subsystems/driving/drive_to_position_controller.hpp"
#include "Gamepad/Better_gamepad.hpp"
#include "task_scheduler.hpp"
#include "subsystems/outtake/gripper.hpp"
#include "subsystems/outtake/arm.hpp"
#include "subsystems/intake/wrist.hpp"
#include "subsystems/intake/extendo.hpp"
#include "subsystems/outtake/wrist.hpp"
#include "subsystems/intake/arm.hpp"
#include "subsystems/intake/swivel.hpp"
#include "subsystems/intake/gripper.hpp"
#include "subsystems/clipper/clipper.hpp"
#include "subsystems/subsystem_utils.hpp"

using namespace sdk;

namespace robot
{
    bool scoring_samples = true;

    Lynx_module control_hub;
    Lynx_module expansion_hub;

    BNO055IMU imu;

    Webcam_name webcam;

    Vision_portal vision_portal;
    Sample_detection *sample_detection;

    int threads_still_running;

    void init(bool do_initial_reset)
    {
        telemetry::set_display_format(telemetry::Display_format::HTML);
        match_data::current_match::setup_variables_for_match();
        control_hub = hardware_map::get<Lynx_module>("Control Hub");
        control_hub.set_bulk_caching_mode(Lynx_module::Bulk_caching_mode::MANUAL);
        expansion_hub = hardware_map::get<Lynx_module>("Expansion Hub 2");
        expansion_hub.set_bulk_caching_mode(Lynx_module::Bulk_caching_mode::MANUAL);

        imu = hardware_map::get<BNO055IMU>("imu");
        BNO055IMU::Parameters parameters;
        parameters.angle_unit = BNO055IMU::Angle_unit::RADIANS;
        parameters.accel_unit = BNO055IMU::Accel_unit::METERS_PERSEC_PERSEC;
        imu.initialize(parameters);

        webcam = hardware_map::get<Webcam_name>("Webcam 1");

        drivetrain::init();
        outtake::init();
        intake::init();
        pinpoint::init();
        // drive_to_position_controller::init();
        ::gamepads::init();

        if (!do_initial_reset)
            return;

        outtake::reset();
        intake::reset();
        shared_ptr<bool> finished_reset = make_shared<bool>(false);

        thread([finished_reset]
               {
            while (!(*finished_reset) && !linear_op_mode::is_stop_requested())
                task_scheduler::execute(); 
            if (linear_op_mode::is_stop_requested())
                stop(); })
            .detach();

        outtake::wait_for_movement_finish();
        intake::wait_for_movement_finish();
        *finished_reset = true;
    }

    void stop()
    {
        if (sample_detection)
        {
            delete sample_detection;
            sample_detection = nullptr;
        }

        task_scheduler::terminate_all_threads();

        drive_to_position_controller::stop();
        outtake::stop();
        intake::stop();

        while (threads_still_running != 0)
            ;
    }

    bool transfer(bool wait_until_done)
    {
        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, []
                                                             {
            if (outtake::move_to_transfer_position(true) &&
                intake::move_to_transfer_position(true) &&
                outtake::gripper::close(true) &&
                outtake::arm::set_rotation(outtake::BASKET_ROTATION, true) &&
                outtake::gripper::close_losely() &&
                outtake::wrist::set_rotation(80_deg))
                intake::wrist::set_rotation(0_deg); }, []
                                                             {
                outtake::gripper::wait_for_movement_finish();
                outtake::wrist::wait_for_movement_finish();
                intake::wrist::wait_for_movement_finish(); });

        return outtake::arm::target_rotation == outtake::BASKET_ROTATION && outtake::elevator::target_elevation == 0_mm && outtake::arm::linkage::target_extension == 0_mm && outtake::wrist::manual_set_target_rotation == 80_deg && outtake::gripper::status == outtake::gripper::Status::CLOSED_LOSELY &&
               intake::extendo::target_extension == intake::TRANSFER_EXTENSION && intake::arm::target_rotation == intake::arm::TRANSFER_ROTATION && intake::swivel::target_rotation == 0_deg && intake::wrist::manual_set_target_rotation == 0_deg && intake::gripper::swivel::manual_set_target_rotation == 0_deg && !intake::gripper::closed;
    }

    bool ready_for_transfer()
    {
        return intake::ready_for_transfer() && outtake::ready_for_transfer();
    }

    void wait_for_movement_finish()
    {
        while (intake::arm::moving || intake::swivel::moving || intake::extendo::moving || intake::wrist::moving || intake::gripper::moving || intake::gripper::swivel::moving || outtake::elevator::moving || outtake::arm::moving || outtake::arm::linkage::moving || outtake::wrist::moving || outtake::gripper::moving || threads_still_running != 0)
        {
            intake::wait_for_movement_finish();
            outtake::wait_for_movement_finish();
        }
    }
} // robot