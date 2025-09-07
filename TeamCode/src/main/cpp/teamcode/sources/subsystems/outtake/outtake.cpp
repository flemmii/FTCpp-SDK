#include "subsystems/outtake/outtake.hpp"

#include <future>

#include "Units-master/Maths.h"
#include "sdk.hpp"

#include "subsystems/outtake/arm.hpp"
#include "subsystems/outtake/elevator.hpp"
#include "subsystems/outtake/gripper.hpp"
#include "subsystems/outtake/wrist.hpp"
#include "subsystems/intake/arm.hpp"
#include "subsystems/intake/intake.hpp"
#include "robot.hpp"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/clipper/aligner.hpp"

using namespace sdk;

namespace outtake
{
    Degree BASKET_ROTATION;

    Touch_sensor spec_aligner_beam;
    sem_t sem_wait_for_spec_in_aligner;
    int threads_wait_for_spec_in_aligner = 0;
    bool already_running_wait_for_spec_in_aligner = false;

    int threads_running = 0;

    void init()
    {
        threads_running = 0;
        wrist::init();
        elevator::init();
        arm::init();
        gripper::init();

        spec_aligner_beam = hardware_map::get<Touch_sensor>("spec_aligner_beam");
        sem_init(&sem_wait_for_spec_in_aligner, 0, 0);

        const auto cache = conv_to_elevation_and_rotation(HIGH_BASKET_HEIGHT, arm::MAX_REACH_EXTENSION_LIMIT, arm::MAX_LENGTH);
        BASKET_ROTATION = get<1>(cache);
    }

    void stop()
    {
        threads_running = 0;
        gripper::stop();
        arm::stop();
        elevator::stop();
        wrist::stop();

        while (threads_wait_for_spec_in_aligner > 0)
            sem_post(&sem_wait_for_spec_in_aligner);

        sem_destroy(&sem_wait_for_spec_in_aligner);

        while (threads_running != 0)
            ;
    }

    Millimetre get_height()
    {
        return arm::calc_height(arm::get_rotation()) + elevator::get_elevation();
    }

    tuple<Millimetre, Degree> conv_to_elevation_and_rotation(const Millimetre &height, const Millimetre &reach, const Millimetre &arm_length, bool extend_arm_when_needed)
    {
        const Millimetre distance_pivot_point_goal = maths::Sqrt(height * height + reach * reach);

        const Degree reach_height_angle = maths::Arctan(-reach, height);
        const Degree angle_elevator_goal = reach_height_angle + elevator::ANGLE;

        const Millimetre distance_pivot_point_goal_sinus = distance_pivot_point_goal * maths::Sin(angle_elevator_goal);
        const Millimetre distance_pivot_point_goal_sinus_abs = abs(distance_pivot_point_goal_sinus);
        const Millimetre distance_pivot_point_goal_cosinus = distance_pivot_point_goal * maths::Cos(angle_elevator_goal);

        // Case when the arm is 90° related to the elevator -> longest reach
        if (distance_pivot_point_goal_sinus_abs > arm_length)
        {
            if (extend_arm_when_needed)
            {
                if (distance_pivot_point_goal_sinus_abs > arm::MAX_LENGTH)
                {
                    log(ANDROID_LOG_WARN, "outtake") << "The arm is not long enough to reach the goal position" << endl;
                    arm::linkage::set_length(arm::linkage::MAX_EXTENSION);
                }
                else
                {
                    arm::linkage::set_length(distance_pivot_point_goal_sinus_abs - arm::BASE_LENGTH);
                }
            }

            return {distance_pivot_point_goal_cosinus, elevator::ANGLE + ((angle_elevator_goal > 0_deg) ? 0_deg : 180_deg)};
        }
        else
        {
            const Millimetre current_length_elevator = elevator::get_length();

            // law of cosines with law of sines: a = b * cos(γ) ± √(c² - b² * (sin(γ))²)
            const Millimetre cache = maths::Sqrt(arm_length * arm_length - distance_pivot_point_goal_sinus * distance_pivot_point_goal_sinus);
            const tuple<Millimetre, Millimetre> length_elevator_possibilities(distance_pivot_point_goal_cosinus + cache, distance_pivot_point_goal_cosinus - cache);
            Millimetre length_elevator;

            if (get<1>(length_elevator_possibilities) < 0_mm || (abs(get<0>(length_elevator_possibilities) - current_length_elevator) < abs(get<1>(length_elevator_possibilities) - current_length_elevator) && get<0>(length_elevator_possibilities) < elevator::MAX_LENGTH))
                length_elevator = get<0>(length_elevator_possibilities);
            else
                length_elevator = get<1>(length_elevator_possibilities);

            const Millimetre reach_elevator = elevator::conv_to_extension(length_elevator);
            const Millimetre height_elevator = elevator::conv_to_elevation(length_elevator);
            return {height_elevator, maths::Arctan(height - height_elevator, -(reach - reach_elevator))};
        }
    }

    tuple<Millimetre, Degree> conv_to_elevation_and_rotation(const Millimetre &height, const Millimetre &reach, const Millimetre &arm_length)
    {
        return conv_to_elevation_and_rotation(height, reach, arm_length, false);
    }

    tuple<Millimetre, Degree> conv_to_elevation_and_rotation(const Millimetre &height, const Millimetre &reach)
    {
        return conv_to_elevation_and_rotation(height, reach, outtake::arm::get_length(), true);
    }

    tuple<Degree, Millimetre> conv_to_rotation_and_length(const Millimetre &height_arm, const Millimetre &reach_arm)
    {
        return {maths::Arctan(height_arm, -reach_arm), maths::Sqrt(height_arm * height_arm + reach_arm * reach_arm) - arm::BASE_LENGTH};
    }

    bool reset(bool wait_until_done)
    {
        wrist::set_rotation(wrist::TRANSFER_ROTATION);
        gripper::open();

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, []
                                                             {
            if (arm::linkage::retract(true) &&
                arm::set_rotation(40_deg, true))
            {
                arm::method_count++;
                elevator::reset_position();
            } }, []
                                                             {
                wrist::wait_for_movement_finish();
                gripper::wait_for_movement_finish();
                elevator::wait_for_movement_finish();
                arm::linkage::wait_for_movement_finish(); });

        return wrist::manual_set_target_rotation + wrist::coaxial_target_rotation == wrist::TRANSFER_ROTATION && gripper::status != gripper::Status::OPEN && elevator::target_elevation == 0_mm && arm::target_rotation == 0_deg;
    }

    bool set_position(Millimetre height, Millimetre reach, Degree rotation_wrist, bool wait_until_done)
    {
        const auto [elevation, rotation_arm] = conv_to_elevation_and_rotation(height, reach);
        elevator::set_elevation(elevation);
        arm::set_rotation(rotation_arm);
        wrist::set_rotation(rotation_wrist);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [] {}, []
                                                             {
                                                                elevator::wait_for_movement_finish();
                                                                arm::wait_for_movement_finish();
                                                                arm::linkage::wait_for_movement_finish();
                                                                wrist::wait_for_movement_finish(); });

        return elevator::target_elevation == elevation && arm::target_rotation == rotation_arm && wrist::manual_set_target_rotation == rotation_wrist;
    }

    bool move_to_transfer_position(bool wait_until_done)
    {
        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, []
                                                             {
            if (gripper::open(true) && 
                wrist::rotate_to_transfer(true) &&
                elevator::retract() &&  
                arm::linkage::retract(true))
                arm::set_rotation(arm::TRANSFER_ROTATION); }, []
                                                             {
                arm::wait_for_movement_finish();
                elevator::wait_for_movement_finish(); });

        return arm::linkage::target_extension == 0_mm && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && arm::target_rotation == arm::TRANSFER_ROTATION && elevator::target_elevation == 0_mm;
    }

    bool move_to_fast_transfer_position(bool wait_until_done)
    {
        const auto cache = conv_to_elevation_and_rotation(robot::DISTANCE_OUTTAKE_INTAKE.z + intake::arm::LENGTH - 30_mm, -(robot::DISTANCE_OUTTAKE_INTAKE.y - 170_mm), arm::MAX_LENGTH);
        const Millimetre elevation = get<0>(cache);
        const Degree rotation = get<1>(cache);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [elevation, rotation]
                                                             {
            if (elevator::set_elevation(elevation) && 
                arm::linkage::set_length(0_mm, true) && 
                wrist::set_rotation(wrist::TRANSFER_ROTATION) && 
                arm::set_rotation(rotation, true))
                arm::linkage::set_length(arm::linkage::MAX_EXTENSION); }, []
                                                             {
                arm::linkage::wait_for_movement_finish();
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish(); });

        return arm::linkage::target_extension == arm::linkage::MAX_EXTENSION && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && arm::target_rotation == rotation && elevator::target_elevation == elevation;
    }

    bool move_to_rest_position(bool wait_until_done)
    {
        wrist::set_rotation(wrist::TRANSFER_ROTATION);
        arm::linkage::retract();
        gripper::open();
        arm::set_rotation(30_deg);
        elevator::retract();

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [] {}, []
                                                             {
            wrist::wait_for_movement_finish();
            gripper::wait_for_movement_finish();
            arm::wait_for_movement_finish();
            arm::linkage::wait_for_movement_finish();
            elevator::wait_for_movement_finish(); });

        return wrist::manual_set_target_rotation + wrist::coaxial_target_rotation == wrist::TRANSFER_ROTATION && gripper::status != gripper::Status::OPEN && elevator::target_elevation == 0_mm && arm::target_rotation == 0_deg;
    }

    bool move_to_high_basket_position(bool wait_until_done)
    {
        const auto result = conv_to_elevation_and_rotation(HIGH_BASKET_HEIGHT, arm::MAX_REACH_EXTENSION_LIMIT, arm::MAX_LENGTH);
        const auto elevation = std::get<0>(result);
        const auto rotation_arm = std::get<1>(result);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [rotation_arm, elevation]
                                                             {
            if (gripper::close_losely(true) &&
                elevator::set_elevation(elevation) &&
                arm::linkage::retract(true) &&
                wrist::set_rotation(80_deg) &&
                arm::set_rotation(rotation_arm, true) &&
                arm::linkage::set_length(arm::linkage::MAX_EXTENSION, true))
                wrist::set_rotation(130_deg); }, []
                                                             {
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish(); });

        return arm::linkage::target_extension == arm::linkage::MAX_EXTENSION && wrist::manual_set_target_rotation == 130_deg && arm::target_rotation == rotation_arm && elevator::target_elevation == elevation;
    }

    bool move_to_low_basket_position(bool wait_until_done)
    {
        const auto result = conv_to_elevation_and_rotation(LOW_BASKET_HEIGHT, arm::MAX_REACH_EXTENSION_LIMIT, arm::BASE_LENGTH);
        const auto elevation = std::get<0>(result);
        const auto rotation_arm = std::get<1>(result);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [rotation_arm, elevation]
                                                             {
            if (elevator::set_elevation(elevation) && 
                arm::linkage::retract(true) && 
                wrist::set_rotation(130_deg))
                arm::set_rotation(rotation_arm); }, []
                                                             {
                arm::wait_for_movement_finish();
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish(); });

        return arm::linkage::target_extension == 0_mm && wrist::manual_set_target_rotation == 130_deg && arm::target_rotation == rotation_arm && elevator::target_elevation == elevation;
    }

    const Millimetre HIGH_CHAMBER_ELEVATOR_REACH = elevator::conv_to_extension(elevator::conv_elevation_to_length(HIGH_CHAMBER_ELEVATION));

    bool get_specimen_and_move_to_high_chamber_position(bool wait_until_done)
    {
        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, []
                                                             {
            if (gripper::close_losely(true) && 
                arm::linkage::retract(true) &&
                wrist::set_rotation(-45_deg) && 
                elevator::set_elevation(HIGH_CHAMBER_ELEVATION) && 
                arm::set_rotation(10_deg, true))
            {
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish();
                arm::linkage::overcenter();
            } }, arm::linkage::wait_for_movement_finish);

        return arm::target_rotation == 10_deg && elevator::target_elevation == HIGH_CHAMBER_ELEVATION && wrist::manual_set_target_rotation == -60_deg;
    }

    bool get_specimen_and_move_to_high_chamber_position_auto(bool wait_until_done)
    {
        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, []
                                                             {
            if (gripper::close(true) && 
                arm::linkage::retract(true) &&
                wrist::set_rotation(-45_deg) && 
                elevator::set_elevation(HIGH_CHAMBER_ELEVATION) && 
                arm::set_rotation(7_deg, true))
            {
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish();
                arm::linkage::overcenter();
            } }, arm::linkage::wait_for_movement_finish);

        return arm::target_rotation == 7_deg && elevator::target_elevation == HIGH_CHAMBER_ELEVATION && wrist::manual_set_target_rotation == -60_deg;
    }

    bool move_to_specimen_intake_position(bool wait_until_done)
    {
        const Millimetre height_arm = SPECIMEN_INTAKE_HEIGHT - HIGH_CHAMBER_ELEVATION;
        const Millimetre reach_arm = SPECIMEN_INTAKE_REACH - HIGH_CHAMBER_ELEVATOR_REACH;
        const auto cache = conv_to_rotation_and_length(height_arm, reach_arm);
        const Degree rotation_arm = get<0>(cache);
        const Millimetre length_arm = get<1>(cache);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [rotation_arm, length_arm]
                                                             {
            if (gripper::open(true) && 
                arm::linkage::retract(true) && 
                elevator::set_elevation(HIGH_CHAMBER_ELEVATION) && 
                wrist::set_rotation(-180_deg) && 
                arm::set_rotation(rotation_arm + 20_deg, true) && 
                arm::linkage::set_length(length_arm, true))
            {
                arm::max_motor_power = 0.15_power;
                arm::set_rotation(rotation_arm, true);
                arm::max_motor_power = 1_power;
            } }, []
                                                             {
                arm::linkage::wait_for_movement_finish();
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish(); });

        return wrist::manual_set_target_rotation == -180_deg && arm::target_rotation == rotation_arm && elevator::target_elevation == HIGH_CHAMBER_ELEVATION;
    }

    bool move_to_specimen_intake_position_auto(bool wait_until_done)
    {
        const Millimetre height_arm = SPECIMEN_INTAKE_HEIGHT - HIGH_CHAMBER_ELEVATION;
        const Millimetre reach_arm = SPECIMEN_INTAKE_REACH - HIGH_CHAMBER_ELEVATOR_REACH - 10_mm;
        const auto cache = conv_to_rotation_and_length(height_arm, reach_arm);
        const Degree rotation_arm = get<0>(cache);
        const Millimetre length_arm = get<1>(cache);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [rotation_arm, length_arm]
                                                             {
            if (gripper::open(true) && 
                arm::linkage::retract(true) && 
                elevator::set_elevation(HIGH_CHAMBER_ELEVATION) && 
                wrist::set_rotation(-180_deg) && 
                arm::set_rotation(rotation_arm + 20_deg, true) && 
                arm::linkage::set_length(length_arm, true))
            {
                arm::max_motor_power = 0.15_power;
                arm::set_rotation(rotation_arm, true);
                arm::max_motor_power = 1_power;
            } }, []
                                                             {
                arm::linkage::wait_for_movement_finish();
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish(); });

        return wrist::manual_set_target_rotation == -180_deg && arm::target_rotation == rotation_arm && elevator::target_elevation == HIGH_CHAMBER_ELEVATION;
    }

    bool move_to_in_sub_transfer_position(bool wait_until_done)
    {
        const auto result = conv_to_elevation_and_rotation(robot::DISTANCE_OUTTAKE_INTAKE.z + intake::arm::LENGTH, -(robot::DISTANCE_OUTTAKE_INTAKE.y + intake::EXTENSION_IN_SUB_TRANSFER - 170_mm), arm::MAX_LENGTH);
        const auto elevation = std::get<0>(result);
        const auto rotation_arm = std::get<1>(result);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [elevation, rotation_arm]
                                                             {
            if (gripper::open(true) && 
                arm::linkage::retract(true) && 
                elevator::set_elevation(elevation) && 
                wrist::set_rotation(wrist::TRANSFER_ROTATION) && 
                arm::set_rotation(rotation_arm, true))
            {
                elevator::wait_for_movement_finish();
                arm::linkage::set_length(arm::linkage::MAX_EXTENSION);
            } }, []
                                                             {
                arm::linkage::wait_for_movement_finish();
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish(); });

        return arm::linkage::target_extension == arm::linkage::MAX_EXTENSION && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && arm::target_rotation == rotation_arm && elevator::target_elevation == elevation;
    }

    bool
    clip(bool wait_until_done)
    {
        const auto result = conv_to_elevation_and_rotation(CLIPPING_HEIGHT, CLIPPING_REACH, arm::MAX_LENGTH);
        const auto elevation = std::get<0>(result);
        const auto rotation_arm = std::get<1>(result);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [rotation_arm, elevation]
                                                             {
            if (gripper::close_losely(true) && 
                arm::linkage::retract(true) &&
                outtake::gripper::close(true) &&
                elevator::set_elevation(elevation + 30_mm) && 
                wrist::set_rotation(0_deg, false, 0_deg) && 
                arm::set_rotation(rotation_arm + 20_deg, true) &&
                arm::linkage::set_length(arm::linkage::MAX_EXTENSION, true))
            {
                elevator::wait_for_movement_finish();
                wrist::wait_for_movement_finish();

                arm::max_motor_power = 0.3_power;
                elevator::max_motor_power = 0.3_power;
                if (arm::set_rotation(rotation_arm, true) && 
                    elevator::set_elevation(elevation, true))
                {
                    arm::max_motor_power = 1_power;
                    elevator::max_motor_power = 1_power;
                    if (wrist::set_rotation(wrist::MAX_ROTATION, true, 0_deg, true))
                    {
                        gripper::close_losely();
                        if (arm::linkage::retract(true))
                            clipper::aligner::setup_for_clips(clipper::aligner::current_clip_count - 1);

                    }
                }
            } }, []
                                                             {
                arm::linkage::wait_for_movement_finish();
                gripper::wait_for_movement_finish(); });

        return wrist::manual_set_target_rotation == wrist::MAX_ROTATION && arm::target_rotation == rotation_arm && arm::linkage::target_extension == 0_mm && elevator::target_elevation == elevation;
    }

    bool clip_and_score(bool wait_until_done)
    {
        shared_ptr<bool> success = make_shared<bool>(false);

        subsystem_utils::execute_function_or_register_thread(threads_running, wait_until_done, [success]
                                                             {
            if (clip(true) && 
                get_specimen_and_move_to_high_chamber_position(true) && 
                outtake::gripper::open(true) && 
                move_to_in_sub_transfer_position(true))
                *success = true; });

        return *success;
    }

    bool is_spec_in_aligner()
    {
        return spec_aligner_beam.is_pressed();
    }

    bool grab_spec_when_in_aligner(bool wait_until_done)
    {
        if (!already_running_wait_for_spec_in_aligner)
        {
            already_running_wait_for_spec_in_aligner = true;
            task_scheduler::register_task(is_spec_in_aligner, 50ms, true,
                                          []
                                          { already_running_wait_for_spec_in_aligner = false;
                                            get_specimen_and_move_to_high_chamber_position(); });
        }

        if (wait_until_done)
            subsystem_utils::wait_for_movement_finish(already_running_wait_for_spec_in_aligner, threads_wait_for_spec_in_aligner, sem_wait_for_spec_in_aligner);

        return true;
    }

    void wait_for_movement_finish()
    {
        while (elevator::moving || arm::moving || arm::linkage::moving || wrist::moving || gripper::moving || threads_running != 0)
        {
            elevator::wait_for_movement_finish();
            arm::wait_for_movement_finish();
            arm::linkage::wait_for_movement_finish();
            wrist::wait_for_movement_finish();
            gripper::wait_for_movement_finish();
        }
    }

    bool ready_for_transfer()
    {
        return elevator::target_elevation == 0_mm && !elevator::moving && arm::target_rotation == arm::TRANSFER_ROTATION && !arm::moving && arm::linkage::target_extension == arm::linkage::MAX_EXTENSION && !arm::linkage::moving && wrist::manual_set_target_rotation == wrist::TRANSFER_ROTATION && !wrist::moving && gripper::status == gripper::Status::OPEN && !gripper::moving;
    }
} // outtake