#include "opmodes/autonomous/sample_preset.hpp"

#include "opmodes/include_all_header.hpp"
#include "opmodes/autonomous/Auto.hpp"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace drive_to_position_controller;
using namespace outtake;
using namespace intake;

namespace Auto::sample
{
    constexpr Position<> basket(415_mm, 415_mm, 45_deg);
    constexpr chrono::steady_clock::duration timeout_for_hang = 2s;

    tuple<Position<>, Vec2d<false>> calc_sample_robot_pos_and_intake_pos(const Location<> &distance_to_first_sample, const Vec2d<false> &intake_extension)
    {
        const Millimetre intake_radius = intake_extension.total_distance();
        bool distance_to_sample_larger_than_intake_radius = distance_to_first_sample.coordinates > intake_radius;
        const Millimetre overshoot = distance_to_first_sample.coordinates - intake_radius;
        const Position<> robot_pos(distance_to_sample_larger_than_intake_radius ? basket.x + overshoot * maths::Sin(distance_to_first_sample.rotation) : basket.x,
                                   distance_to_sample_larger_than_intake_radius ? basket.y + overshoot * maths::Cos(distance_to_first_sample.rotation) : basket.y,
                                   distance_to_first_sample.rotation);
        const Vec2d<false> intake_pos = distance_to_sample_larger_than_intake_radius ? intake_extension : intake_extension * (1 + (overshoot / intake_radius));
        return {robot_pos, intake_pos};
    }

    bool score_sample_and_get_next(int iteration_count, const Degree &rotation, bool skip_scoring = false)
    {
        if (!skip_scoring)
        {
            drive_to_position(2 * iteration_count, basket.rotation);
            robot::wait_for_movement_finish();
            outtake::elevator::set_elevation(300_mm);
            wait_until_position_is_reached(3000ms);
            outtake::move_to_high_basket_position(true);
            outtake::gripper::open(true);
            outtake::move_to_transfer_position();
        }

        intake::set_position({intake::arm::GRIPPING_RADIUS, 0_mm}, intake::arm::HOVERING_ROTATION, 90_deg, -rotation);
        drive_to_position(2 * iteration_count + 1, rotation, 3000ms);
        intake::wait_for_movement_finish();

        if (!intake::grip_sample(true, false))
            return true;

        if (iteration_count == 2)
        {
            drive_to_position(6, basket.rotation);
            sleep(300ms);
        }

        outtake::wait_for_movement_finish();
        robot::transfer();

        return false;
    }

    void search_for_sample_and_score()
    {
        drive_to_position(7, true, 3s);
        while (30s - (chrono::steady_clock::now() - start_time) < timeout_for_hang)
        {
            if (intake::search_sample({intake::arm::GRIPPING_RADIUS, 0_mm}, 45_deg, true))
                break;
        }

        intake::move_to_transfer_position();
        drive_to_position(8, false);
        intake::wait_for_movement_finish();
        outtake::gripper::close_losely(true);
        outtake::move_to_high_basket_position();
        wait_until_position_is_reached(4s);

        outtake::wait_for_movement_finish();
        outtake::gripper::open(true);
    }

    void auton()
    {
        constexpr Vec3d center_sample_field_vec3d = field::samples::neutral::own_alliance::field.center();
        constexpr Vec2d<> center_sample_field(center_sample_field_vec3d.x, center_sample_field_vec3d.y);
        constexpr Vec2d<> center_sample_middle(center_sample_field.x - distance_samples, center_sample_field.y);
        constexpr Vec2d<> center_sample_wall(center_sample_middle.x - distance_samples, center_sample_middle.y);

        constexpr Position<> first_sample_grabbing(center_sample_field.x - 50_mm, center_sample_field.y - intake::arm::GRIPPING_RADIUS - robot::SIZE.y / 2 + 40_mm, 0_deg);
        constexpr Position<> second_sample_grabbing(center_sample_middle.x - 30_mm, center_sample_middle.y - intake::arm::GRIPPING_RADIUS - robot::SIZE.y / 2 + 40_mm, 0_deg);
        constexpr Position<> third_sample_grabbing(center_sample_wall.x - 30_mm + intake::arm::GRIPPING_RADIUS + robot::SIZE.y / 2 - 60_mm, center_sample_wall.y, -90_deg);
        const vector<Position<>> sample_grabbing_pos = {first_sample_grabbing, second_sample_grabbing, third_sample_grabbing};

        constexpr Vec3d submersible_barrier = field::submersible::barrier::left.center();
        constexpr Vec2d<> intaking_at_submersible = {submersible_barrier.x - robot::SIZE.y / 2 - 145_mm, submersible_barrier.y};

        starting_position = {field::field_tiles.top_right.x + robot::SIZE.x / 2, robot::SIZE.y / 2, 0_deg};

        predefine_sequential_paths({{starting_position.to_vec(),
                                     {starting_position.x, basket.y},
                                     basket.to_vec()},
                                    // 1. sample
                                    {first_sample_grabbing.to_vec()},
                                    {basket.to_vec() + 10_mm},
                                    // 2. sample
                                    {second_sample_grabbing.to_vec()},
                                    {basket.to_vec() + 20_mm},
                                    // 3. sample
                                    {third_sample_grabbing.to_vec()},
                                    {basket.to_vec()},
                                    // More samples ????
                                    {basket.to_vec() + 100_mm,
                                     {basket.x + 100_mm, intaking_at_submersible.y},
                                     intaking_at_submersible},
                                    {{basket.x + 100_mm, intaking_at_submersible.y},
                                     basket.to_vec() + 100_mm,
                                     basket.to_vec()},
                                    // lvl 1. ascend
                                    {{660_mm, 490_mm},
                                     {170_mm, intaking_at_submersible.y},
                                     {1070_mm, intaking_at_submersible.y}}});

        outtake::gripper::close_losely();
        finish_init_and_wait_for_start();

        if (!op_mode_is_active())
            return;

        task_scheduler::register_thread([=]
                                        {
            bool skip_scoring = false;
            auto it = sample_grabbing_pos.begin();
            for (int i = 0; i < 3; i++, ++it)
            {
                const Position robot_pos = *it;
                skip_scoring = score_sample_and_get_next(i, robot_pos.rotation, skip_scoring);
            }

            if (!skip_scoring)
            {
                robot::wait_for_movement_finish();
                outtake::elevator::set_elevation(300_mm);
                wait_until_position_is_reached(3000ms);
                outtake::move_to_high_basket_position(true);
                outtake::gripper::open(true);
                outtake::move_to_transfer_position();
            }

            /*
            while (30s - (chrono::steady_clock::now() - start_time) < timeout_for_hang)
                search_for_sample_and_score();
                */

            outtake::arm::linkage::retract();
            elevator::retract();
            outtake::arm::set_rotation(0_deg);

            if ((points_on_predefined_curves[9].back() - pinpoint::get_position().to_vec()).total_distance() < max_deviations.coordinates)
                drive_to_position({points_on_predefined_curves[9].back(), 90_deg});
            else
                drive_to_position(9, true);

            wait_until_position_is_reached((30s - (chrono::steady_clock::now() - start_time)) - 2s);
            intake::extendo::set_extension(300_mm);
            drivetrain::drive<false>({0.3_power, 0_power, 0_power});
            elevator::wait_for_movement_finish();
            outtake::arm::wait_for_movement_finish();
            outtake::arm::linkage::set_length(outtake::arm::linkage::MAX_EXTENSION, true);
            outtake::arm::set_power(0.8_power); });
    }
}