#include "subsystems/intake/swivel.hpp"

#include "sdk.hpp"
#include "subsystems/intake/arm.hpp"
#include "subsystems/intake/gripper.hpp"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/intake/intake.hpp"
#include "subsystems/intake/arm.hpp"

using namespace sdk;
using namespace std;

namespace intake::swivel
{
    Servo_impl_ex servo;

    Degree target_rotation = 0_deg;
    Degree last_target_rotation = 0_deg;
    bool moving = false;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        servo = hardware_map::get<Servo>("intake_swivel");
        servo.set_pwm_range(servo_type::PWM_RANGE);
        servo.set_direction(Servo::Direction::REVERSE);

        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
    }

    chrono::steady_clock::time_point start_time_rotation = chrono::steady_clock::now();

    Degree est_current_pos()
    {
        chrono::steady_clock::duration time_elapsed = chrono::steady_clock::now() - start_time_rotation;
        if (time_elapsed > chrono::milliseconds(static_cast<int>(Millisecond(abs(target_rotation - last_target_rotation) / MAX_VELOCITY).Value())))
            return target_rotation;

        return last_target_rotation + MAX_VELOCITY * maths::Sgn(target_rotation - last_target_rotation) * Millisecond(chrono::duration_cast<chrono::milliseconds>(time_elapsed).count());
    }

    chrono::steady_clock::duration time_needed_to_reach_goal(const Degree &target)
    {
        return chrono::milliseconds(static_cast<int>(Millisecond((target - last_target_rotation) / (MAX_VELOCITY * maths::Sgn(target_rotation - last_target_rotation))).Value())) - (chrono::steady_clock::now() - start_time_rotation);
    }

    Millimetre calc_extension_forwards(Degree rotation)
    {
        return arm::GRIPPING_RADIUS * maths::Cos(rotation);
    }

    Millimetre calc_extension_sidewards(Degree rotation)
    {
        return arm::GRIPPING_RADIUS * maths::Sin(rotation);
    }

    Vec2d<false> calc_pos(Degree rotation)
    {
        return {calc_extension_forwards(rotation), calc_extension_sidewards(rotation)};
    }

    bool set_rotation(Degree rotation, bool wait_until_done)
    {
        rotation = clamp(rotation, MIN_ROTATION, MAX_ROTATION);
        if (arm::target_rotation > get<0>(INTERFERENCE_ROTATION_ARM) && arm::target_rotation < get<1>(INTERFERENCE_ROTATION_ARM))
            rotation = clamp(rotation, -INTERFERENCE_ROTATION_SWIVEL, INTERFERENCE_ROTATION_SWIVEL);

        log(ANDROID_LOG_INFO, "intake_swivel") << "Setting rotation to: " << rotation << endl;

        last_target_rotation = target_rotation;
        target_rotation = rotation;

        chrono::steady_clock::duration time_needed_for_arm = 0ms;
        if (arm::moving)
        {
            clog << "10" << endl;
            const Degree current_pos = arm::est_current_pos();
            if ((arm::target_rotation - arm::last_target_rotation) > 0_deg && current_pos < get<1>(INTERFERENCE_ROTATION_ARM))
            {
                clog << "11" << endl;
                time_needed_for_arm = arm::time_needed_to_reach_goal(get<1>(INTERFERENCE_ROTATION_ARM));
            }
            else if ((arm::target_rotation - arm::last_target_rotation) < 0_deg && current_pos > get<0>(INTERFERENCE_ROTATION_ARM))
            {
                clog << "12" << endl;
                time_needed_for_arm = time_needed_to_reach_goal(get<0>(INTERFERENCE_ROTATION_ARM));
            }
            servo.set_position(clamp((0.5 + (INTERFERENCE_ROTATION_SWIVEL * maths::Sgn(rotation) - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));
        }

        const int current_method_count = ++method_count;
        moving = true;

        const function<void()> move_swivel([rotation, current_method_count]
                                           {
            if (current_method_count != method_count)
                return;

                                            clog << "13" << endl;
            gripper::swivel::update_rotation(-rotation);
            servo.set_position(clamp((0.5 + (rotation - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_target_rotation) / MAX_VELOCITY).Value()));
            if (time_needed == 0ms)
            {
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
                return;
            }

            start_time_rotation = chrono::steady_clock::now();
            clog << "14" << endl;

            task_scheduler::run_after([current_method_count, rotation]
                                    {
                                        clog << "15" << endl;     
                    if (current_method_count != method_count)
                        return;

                    log(ANDROID_LOG_INFO, "intake_swivel") << "finished rotating" << endl;
                    moving = false;
                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished); },
                                    time_needed); });

        if (time_needed_for_arm <= 0ms)
            move_swivel();
        else
            task_scheduler::run_after(move_swivel, time_needed_for_arm);

        if (wait_until_done)
            wait_for_movement_finish();

        return current_method_count == method_count;
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }

    bool ready_for_transfer()
    {
        return target_rotation == 0_deg && !moving;
    }
}