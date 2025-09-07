#include "opmodes/autonomous/specimen_preset.hpp"

#include "opmodes/include_all_header.hpp"
#include "opmodes/autonomous/Auto.hpp"

using namespace std;

using namespace sdk;
using namespace linear_op_mode;
using namespace drive_to_position_controller;
using namespace outtake;
using namespace intake;

namespace Auto::specimen
{
    const vector<Vec2d<true, Millimetre>> generate_path_to_chamber(Vec2d<true, Millimetre> offset)
    {
        return {Vec2d<>(field::field_tiles.top_right.x * 3, field::submersible::chamber::own_alliance::high.bottom_left.y - robot::SIZE.y / 2 + 50_mm) + offset};
    }

    const vector<Vec2d<true, Millimetre>> generate_path_to_observation_zone(Vec2d<true, Millimetre> offset)
    {
        return {
            Vec2d<>(field::field_tiles.top_right.x * 5 - 10_in, robot::SIZE.y / 2 + 600_mm) + offset,
            Vec2d<>(field::field_tiles.top_right.x * 5 - 10_in, robot::SIZE.y / 2) + offset};
    }

    void put_sample_into_observation_zone(size_t index, Position<true, Millimetre, Degree> sample_grabbing)
    {
        drive_to_position(index, sample_grabbing.rotation);
        if (index == 1)
        {
            sleep(1000ms);
            intake::set_position({extendo::MAX_EXTENSION, -maths::Sqrt(0.5) * intake::arm::GRIPPING_RADIUS}, intake::arm::HOVERING_ROTATION, 0_deg, 0_deg);
            intake::gripper::swivel::set_rotation(0_deg, false, 0_deg);
            intake::wait_for_movement_finish();
        }

        wait_until_position_is_reached(4000ms);
        if (index == 3)
            extendo::set_extension(extendo::target_extension + 150_mm, true);

        intake::arm::set_rotation(35_deg, true);

        drivetrain::drive<false>({0_power, 0_power, 0.5_power}, 1);
        while (pinpoint::get_position().rotation < 110_deg)
            this_thread::sleep_for(50ms);

        drivetrain::drive<false>({0_power, 0_power, 0_power}, 1);
        intake::arm::set_rotation(intake::arm::HOVERING_ROTATION);

        if (index == 2)
            extendo::set_extension(extendo::target_extension - 150_mm);
    }

    void get_specimen_and_score(int iteration_count)
    {
        max_motor_power = 0.5_power;
        if (iteration_count == 0)
            drive_to_position(2 * iteration_count + 4, false, 4000ms);
        else
            drive_to_position(2 * iteration_count + 4, 0_deg, 4000ms);
        max_motor_power = 1_power;

        outtake::wait_for_movement_finish();
        outtake::arm::set_power(-0.2_power);
        sleep(300ms);
        outtake::gripper::close_losely(true);
        outtake::arm::set_power(0_power);
        outtake::get_specimen_and_move_to_high_chamber_position();
        drive_to_position(2 * iteration_count + 5, 0_deg, 4000ms);
        outtake::gripper::open(true);
        outtake::move_to_specimen_intake_position_auto();
    }

    void auton()
    {
        constexpr Millimetre y_coordinate = 600_mm;
        constexpr Millimetre y_coordinate_third_sample = 800_mm;
        constexpr Vec3d center_sample_field = field::samples::alliance_specific::own_alliance::field.center();
        constexpr Millimetre distance_to_sample = extendo::MAX_EXTENSION + robot::SIZE.y / 2;
        const Position<> first_sample_grabbing(center_sample_field.x - maths::Sqrt(0.5) * distance_to_sample + 80_mm, center_sample_field.y - maths::Sqrt(0.5) * distance_to_sample + 40_mm, 35_deg);
        const Position<> second_sample_grabbing(first_sample_grabbing.x + distance_samples - 20_mm, first_sample_grabbing.y, first_sample_grabbing.rotation);
        const Position<> third_sample_grabbing(second_sample_grabbing.x + distance_samples - 20_mm, second_sample_grabbing.y, second_sample_grabbing.rotation);

        starting_position = {field::field_tiles.top_right.x * 3 + robot::SIZE.x / 2,
                             robot::SIZE.y / 2, 0_deg};

        predefine_sequential_paths({{starting_position.to_vec(),
                                     {starting_position.x - 200_mm, field::submersible::chamber::own_alliance::high.center().y - robot::SIZE.y / 2 + 30_mm}},
                                    {{starting_position.x - 200_mm, 400_mm},
                                     first_sample_grabbing.to_vec()},
                                    {second_sample_grabbing.to_vec()},
                                    {third_sample_grabbing.to_vec()},
                                    // 2nd specimen
                                    {{field::field_tiles.top_right.x * 5 - 10_in + 50_mm, third_sample_grabbing.y},
                                     {field::field_tiles.top_right.x * 5 - 10_in + 50_mm, robot::SIZE.y / 2 + 5_mm}},
                                    generate_path_to_chamber({0_mm, 0_mm}),
                                    // 3rd specimen
                                    generate_path_to_observation_zone({0_mm, 5_mm}),
                                    generate_path_to_chamber({1_in, 0_mm}),
                                    // 4th specimen
                                    generate_path_to_observation_zone({0_mm, 5_mm}),
                                    generate_path_to_chamber({2_in, 0_mm}),
                                    // 5th specimen
                                    generate_path_to_observation_zone({0_mm, 5_mm}),
                                    generate_path_to_chamber({3_in, 0_mm})});

        outtake::wrist::set_rotation(-90_deg);
        sleep(3s);
        outtake::gripper::close_losely();
        finish_init_and_wait_for_start();

        if (!op_mode_is_active())
            return;

        task_scheduler::register_thread([=]
                                        {
            outtake::get_specimen_and_move_to_high_chamber_position();
            deceleration_multiplier = 0.8;
            drive_to_position(0, 0_deg, 2000ms);
            outtake::wait_for_movement_finish();
            outtake::move_to_rest_position();

            put_sample_into_observation_zone(1, first_sample_grabbing);
            put_sample_into_observation_zone(2, second_sample_grabbing);
            //put_sample_into_observation_zone(3, third_sample_grabbing);
            deceleration_multiplier = 0.95;
            extendo::retract();
            intake::swivel::set_rotation(0_deg);
            intake::arm::move_to_transfer_position();
            intake::wrist::set_rotation(intake::wrist::TRANSFER_ROTATION);
            outtake::move_to_specimen_intake_position_auto();

            for (int i = 0; i < 4; i++)
            {
                get_specimen_and_score(i);
                if (30s - (chrono::steady_clock::now() - start_time) < 1s)
                    break;
            }
            drive_to_position({field::field_tiles.top_right.x * 5, robot::SIZE.y / 2, 0_deg}, 2000ms); });
    }
}