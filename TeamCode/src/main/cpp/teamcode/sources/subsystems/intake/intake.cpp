#include "subsystems/intake/intake.hpp"

#include <semaphore.h>

#include "sdk.hpp"

#include "subsystems/intake/extendo.hpp"
#include "subsystems/intake/gripper.hpp"
#include "subsystems/intake/swivel.hpp"
#include "subsystems/intake/arm.hpp"
#include "subsystems/intake/wrist.hpp"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "vision/Sample_detection.hpp"

using namespace std;
using namespace sdk;

namespace intake
{
    int threads_still_running = 0;

    void init()
    {
        arm::init();
        extendo::init();
        gripper::init();
        swivel::init();
        wrist::init();
    }

    void stop()
    {
        arm::stop();
        extendo::stop();
        gripper::stop();
        swivel::stop();
        wrist::stop();

        while (threads_still_running != 0)
            ;
    }

    Millimetre get_extension()
    {
        return swivel::calc_extension_forwards(swivel::target_rotation) + extendo::get_extension();
    }

    tuple<Millimetre, Degree> conv_to_extension_and_rotation(Vec2d<false> pos)
    {
        if (pos.forwards > extendo::MAX_EXTENSION)
            pos.forwards = extendo::MAX_EXTENSION;

        pos.sidewards = clamp(pos.sidewards, -arm::GRIPPING_RADIUS, arm::GRIPPING_RADIUS);
        if (pos.forwards > extendo::MAX_EXTENSION)
            pos.forwards = extendo::MAX_EXTENSION;

        const Millimetre current_extension_extendo = extendo::moving ? extendo::get_extension() : extendo::target_extension;
        Millimetre extension_forwards_swivel = maths::Sqrt(arm::GRIPPING_RADIUS * arm::GRIPPING_RADIUS - pos.sidewards * pos.sidewards);

        const Degree angle_when_reversed = maths::Arctan(pos.sidewards, -extension_forwards_swivel);
        if (pos.forwards - extension_forwards_swivel < 0_mm ||
            (abs(pos.forwards - (current_extension_extendo + extension_forwards_swivel)) > abs(pos.forwards - (current_extension_extendo - extension_forwards_swivel)) &&
             pos.forwards + extension_forwards_swivel < extendo::MAX_EXTENSION && angle_when_reversed > swivel::MIN_ROTATION && angle_when_reversed < swivel::MAX_ROTATION))
            extension_forwards_swivel = -extension_forwards_swivel;

        return {pos.forwards - extension_forwards_swivel, maths::Arctan(pos.sidewards, extension_forwards_swivel)};
    }

    Vec2d<false> get_camera_position()
    {
        const Degree swivel_rotation = swivel::target_rotation;
        const Degree arm_rotation = arm::target_rotation;
        const Millimetre swivel_radius = arm::DISTANCE_SWIVEL_ARM + arm::LENGTH * maths::Cos(arm_rotation);
        const Degree rotation_camera = swivel_rotation + maths::Arctan(CAMERA_OFFSET_SIDEWARDS, swivel_radius);
        const Millimetre camera_radius = maths::Sqrt(swivel_radius * swivel_radius + CAMERA_OFFSET_SIDEWARDS * CAMERA_OFFSET_SIDEWARDS);
        return {extendo::get_extension() + camera_radius * maths::Cos(rotation_camera) + DISTANCE_EXTENDO_SWIVEL, camera_radius * maths::Sin(rotation_camera)};
    }

    bool reset(bool wait_until_done)
    {
        arm::set_rotation(arm::TRANSFER_ROTATION);
        swivel::set_rotation(0_deg);
        wrist::set_rotation(wrist::TRANSFER_ROTATION);
        gripper::open();
        extendo::reset_position();

        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, [] {}, []
                                                             {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::wait_for_movement_finish();
            extendo::wait_for_movement_finish(); });

        return arm::target_rotation == arm::TRANSFER_ROTATION && swivel::target_rotation == 0_deg && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && !gripper::closed && extendo::target_extension == 0_mm;
    }

    bool set_position(Vec2d<false> pos, Degree rotation_arm, Degree rotation_wrist, Degree rotation_gripper, bool wait_until_done)
    {
        const auto [extension, rotation_swivel] = conv_to_extension_and_rotation(pos);
        arm::set_rotation(rotation_arm);
        swivel::set_rotation(rotation_swivel);
        extendo::set_extension(extension);
        wrist::set_rotation(rotation_wrist);
        gripper::swivel::set_rotation(rotation_gripper);

        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, [] {}, []
                                                             {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            extendo::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::swivel::wait_for_movement_finish(); });

        return arm::target_rotation == rotation_arm && swivel::target_rotation == rotation_swivel && extendo::target_extension == extension && wrist::manual_set_target_rotation == rotation_wrist && gripper::swivel::manual_set_target_rotation == rotation_gripper;
    }

    bool move_to_hovering_position(Vec2d<false> pos, bool wait_until_done)
    {
        const auto [extension, rotation_swivel] = conv_to_extension_and_rotation(pos);
        arm::set_rotation(arm::HOVERING_ROTATION);
        swivel::set_rotation(rotation_swivel);
        extendo::set_extension(extension);
        wrist::set_rotation(90_deg, false, 0_deg);
        gripper::swivel::set_rotation(0_deg);

        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, [] {}, []
                                                             {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            extendo::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::swivel::wait_for_movement_finish(); });

        return arm::target_rotation == arm::HOVERING_ROTATION && swivel::target_rotation == rotation_swivel && extendo::target_extension == extension && wrist::manual_set_target_rotation == 90_deg && gripper::swivel::manual_set_target_rotation == 0_deg;
    }

    bool move_to_transfer_position(bool wait_until_done)
    {
        gripper::close();
        arm::set_rotation(arm::TRANSFER_ROTATION);
        wrist::set_rotation(wrist::TRANSFER_ROTATION);
        swivel::set_rotation(0_deg);
        extendo::set_extension(TRANSFER_EXTENSION + 100_mm);
        gripper::swivel::set_rotation(0_deg);

        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, []
                                                             {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::swivel::wait_for_movement_finish();
            gripper::close_losely(true);
            sleep(500ms);
            if (extendo::target_extension == TRANSFER_EXTENSION + 100_mm)
                extendo::set_extension(TRANSFER_EXTENSION); }, extendo::wait_for_movement_finish);

        return arm::target_rotation == arm::TRANSFER_ROTATION && swivel::target_rotation == 0_deg && extendo::target_extension == TRANSFER_EXTENSION && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && gripper::swivel::manual_set_target_rotation == 0_deg;
    }

    bool move_to_fast_transfer_position(bool wait_until_done)
    {
        arm::set_rotation(arm::TRANSFER_ROTATION);
        wrist::set_rotation(wrist::TRANSFER_ROTATION);
        swivel::set_rotation(0_deg);
        extendo::set_extension(100_mm);
        gripper::swivel::set_rotation(0_deg);

        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, []
                                                             {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::swivel::wait_for_movement_finish();
            if (extendo::target_extension == 100_mm)
                extendo::retract(); }, extendo::wait_for_movement_finish);

        return arm::target_rotation == arm::TRANSFER_ROTATION && swivel::target_rotation == 0_deg && extendo::target_extension == 0_mm && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && gripper::swivel::manual_set_target_rotation == 0_deg;
    }

    bool move_to_in_sub_transfer_position(bool wait_until_done)
    {
        arm::set_rotation(arm::TRANSFER_ROTATION);
        wrist::set_rotation(wrist::TRANSFER_ROTATION);
        swivel::set_rotation(0_deg);
        extendo::set_extension(EXTENSION_IN_SUB_TRANSFER + 100_mm);
        gripper::swivel::set_rotation(0_deg);

        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, []
                                                             {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::swivel::wait_for_movement_finish();
            if (extendo::target_extension == EXTENSION_IN_SUB_TRANSFER + 100_mm)
                extendo::set_extension(EXTENSION_IN_SUB_TRANSFER); }, extendo::wait_for_movement_finish);

        return arm::target_rotation == arm::TRANSFER_ROTATION && swivel::target_rotation == 0_deg && extendo::target_extension == EXTENSION_IN_SUB_TRANSFER && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && gripper::swivel::manual_set_target_rotation == 0_deg;
    }

    bool grip_sample(bool wait_until_done, bool check_for_correct_color)
    {
        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, [check_for_correct_color]
                                                             {
            const Degree gripper_swivel_rotation = gripper::swivel::manual_set_target_rotation;
            if (gripper::open(true) &&
                wrist::set_rotation(90_deg, false, 0_deg) &&
                arm::set_rotation(arm::GRIPPING_ROTATION, true) && 
                gripper::close_losely(true) && 
                gripper::swivel::set_rotation(0_deg) && 
                wrist::set_rotation(0_deg) &&
                arm::set_rotation(-40_deg, true))
            {
                wrist::wait_for_movement_finish();
                sleep(100ms);
                swivel::wait_for_movement_finish();
                if ((check_for_correct_color && !gripper::correct_sample()) || (!check_for_correct_color && gripper::get_sample_color() == gripper::Sample_color::NONE))
                {
                    wrist::set_rotation(90_deg, false, 0_deg);
                    gripper::swivel::set_rotation(gripper_swivel_rotation);
                    arm::set_rotation(arm::HOVERING_ROTATION, true);
                    gripper::open();
                }
            } }, []
                                                             {
                gripper::swivel::wait_for_movement_finish();
                gripper::wait_for_movement_finish(); });

        return arm::target_rotation == -40_deg && gripper::closed;
    }

    bool search_sample(Vec2d<false> start_pos, Degree rotation_wrist, bool wait_until_done)
    {
        gripper::open();
        subsystem_utils::execute_function_or_register_thread(threads_still_running, wait_until_done, [start_pos, rotation_wrist]
                                                             {
            if (set_position(start_pos, arm::SEARCHING_ROTATION, rotation_wrist, 0_deg, true))
            {
                gripper::wait_for_movement_finish();
                auto possible_sample_position = robot::sample_detection->get_sample_position();
                if (get<0>(possible_sample_position) && set_position(get<1>(possible_sample_position) + get_camera_position(), arm::GRIPPING_ROTATION + 10_deg, 90_deg, get<2>(possible_sample_position), true) && 
                    arm::set_rotation(arm::GRIPPING_ROTATION, true) &&
                    gripper::close_losely(true))
                    arm::set_rotation(arm::GRIPPING_ROTATION + 10_deg);
                } }, arm::wait_for_movement_finish);

        return arm::target_rotation == arm::GRIPPING_ROTATION + 10_deg && gripper::closed;
    }

    void wait_for_movement_finish()
    {
        while (arm::moving || swivel::moving || extendo::moving || wrist::moving || gripper::moving || gripper::swivel::moving || threads_still_running != 0)
        {
            arm::wait_for_movement_finish();
            swivel::wait_for_movement_finish();
            extendo::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::wait_for_movement_finish();
            gripper::swivel::wait_for_movement_finish();
        }
    }

    bool ready_for_transfer()
    {
        return arm::ready_for_transfer() && swivel::ready_for_transfer() && extendo::ready_for_transfer() && wrist::ready_for_transfer() && gripper::swivel::ready_for_transfer();
    }
} // intake

Millimetre intake::get_camera_height()
{
    // minus because moving to the robot the angle is negative
    Millimetre hight_from_axle_to_cam = -maths::Sin(intake::arm::target_rotation) * intake::arm::LENGTH;
    return intake::arm::AXLE_GROUND_HEIGHT + hight_from_axle_to_cam;
}

Degree intake::get_camera_angle()
{
    return 90_deg - intake::wrist::manual_set_target_rotation;
}
