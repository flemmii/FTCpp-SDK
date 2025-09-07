#include "subsystems/intake/arm.hpp"

#include "sdk.hpp"
#include "constants.hpp"
#include "subsystems/intake/wrist.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/intake/swivel.hpp"
#include "subsystems/intake/intake.hpp"

using namespace sdk;
using namespace std;

namespace intake::arm
{
    Servo_impl_ex servo;

    Degree target_rotation = -90_deg;
    Degree last_target_rotation = -90_deg;
    bool moving = false;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        servo = hardware_map::get<Servo>("intake_arm");
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

    bool set_rotation(Degree rotation, bool wait_until_done)
    {
        rotation = clamp(rotation, MIN_ROTATION, MAX_ROTATION);
        log(ANDROID_LOG_INFO, "intake_arm") << "Setting rotation to: " << rotation << endl;

        last_target_rotation = target_rotation;
        target_rotation = rotation;

        const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_target_rotation) / MAX_VELOCITY).Value()));

        const Degree stored_swivel_rotation = swivel::target_rotation;
        int swivel_method_count = -100;
        chrono::steady_clock::duration time_needed_for_swivel = 0ms;
        const bool arm_pos_vel_and_interfering = target_rotation > get<1>(INTERFERENCE_ROTATION_ARM) && last_target_rotation < get<0>(INTERFERENCE_ROTATION_ARM);
        const bool arm_neg_vel_and_interfering = last_target_rotation > get<1>(INTERFERENCE_ROTATION_ARM) && target_rotation < get<0>(INTERFERENCE_ROTATION_ARM);

        if (abs(stored_swivel_rotation) > INTERFERENCE_ROTATION_SWIVEL)
        {
            clog << "187" << endl;
            if (rotation < get<0>(INTERFERENCE_ROTATION_ARM) && rotation > get<1>(INTERFERENCE_ROTATION_ARM))
            {
                swivel::set_rotation(INTERFERENCE_ROTATION_SWIVEL * maths::Sgn(swivel::target_rotation));
            }
            else if (arm_pos_vel_and_interfering || arm_neg_vel_and_interfering)
            {
                const Degree current_rotation_swivel = swivel::est_current_pos();
                if ((stored_swivel_rotation > INTERFERENCE_ROTATION_SWIVEL && current_rotation_swivel < INTERFERENCE_ROTATION_SWIVEL) ||
                    (stored_swivel_rotation < -INTERFERENCE_ROTATION_SWIVEL && current_rotation_swivel > -INTERFERENCE_ROTATION_SWIVEL))
                {
                    clog << "test 1" << endl;
                    log(ANDROID_LOG_INFO, "intake_arm") << "moving swivel out of the way" << endl;
                    swivel::set_rotation(INTERFERENCE_ROTATION_SWIVEL * maths::Sgn(stored_swivel_rotation));
                    swivel_method_count = swivel::method_count;
                }
                else if ((stored_swivel_rotation > INTERFERENCE_ROTATION_SWIVEL && current_rotation_swivel > INTERFERENCE_ROTATION_SWIVEL) ||
                         (stored_swivel_rotation < -INTERFERENCE_ROTATION_SWIVEL && current_rotation_swivel < -INTERFERENCE_ROTATION_SWIVEL))
                {
                    clog << "test 2" << endl;
                    log(ANDROID_LOG_INFO, "intake_arm") << "moving swivel out of the way" << endl;
                    swivel::set_rotation(INTERFERENCE_ROTATION_SWIVEL * maths::Sgn(stored_swivel_rotation));
                    swivel_method_count = swivel::method_count;
                    time_needed_for_swivel = swivel::time_needed_to_reach_goal(INTERFERENCE_ROTATION_SWIVEL * maths::Sgn(stored_swivel_rotation));
                }
                else
                {
                    clog << "test 3" << endl;
                    time_needed_for_swivel = swivel::time_needed_to_reach_goal(INTERFERENCE_ROTATION_SWIVEL * maths::Sgn(stored_swivel_rotation));
                }

                servo.set_position(clamp((0.5 + ((arm_pos_vel_and_interfering ? get<0>(INTERFERENCE_ROTATION_ARM) : get<1>(INTERFERENCE_ROTATION_ARM)) + OFFSET - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));
            }
        }

        const int current_method_count = ++method_count;
        moving = true;

        const function<void()> move_arm([=]
                                        {
            if (current_method_count != method_count)
                return;
                                            clog << "test 4" << endl;
            wrist::update_rotation(-rotation);
            servo.set_position(clamp((0.5 + (rotation + OFFSET - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            start_time_rotation = chrono::steady_clock::now();

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_target_rotation) / MAX_VELOCITY).Value()));
            if (time_needed == 0ms)
            {
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
                return;
            }
            
            task_scheduler::run_after([current_method_count, rotation, stored_swivel_rotation]
                                      {
                    clog << "test 4281978" << endl;
                    if (current_method_count != method_count)
                        return;

                    log(ANDROID_LOG_INFO, "intake_arm") << "finished rotating" << endl;
                    moving = false;
                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished); },
                                      time_needed);

            if (swivel::method_count == swivel_method_count)
            {
                clog << "hahah" << endl;
                task_scheduler::run_after([stored_swivel_rotation, swivel_method_count, current_method_count]
                                          {
                    if (swivel::method_count != swivel_method_count || current_method_count != method_count)
                        return;

                    log(ANDROID_LOG_INFO, "intake_arm") << "moving swivel back" << endl;

                    swivel::set_rotation(stored_swivel_rotation); }, time_needed_to_reach_goal(arm_pos_vel_and_interfering ? get<0>(INTERFERENCE_ROTATION_ARM) : get<1>(INTERFERENCE_ROTATION_ARM)));
            } });

        if (time_needed_for_swivel <= 0ms)
            move_arm();
        else
            task_scheduler::run_after(move_arm, time_needed_for_swivel);

        if (wait_until_done)
            wait_for_movement_finish();

        return current_method_count == method_count;
    }

    bool move_to_transfer_position(bool wait_until_done)
    {
        return set_rotation(TRANSFER_ROTATION, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }

    bool ready_for_transfer()
    {
        return target_rotation == TRANSFER_ROTATION && !moving;
    }
}