#include "subsystems/clipper/intake.hpp"

#include <semaphore.h>
#include "Units-master/Maths.h"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"

using namespace sdk;
using namespace std;

namespace clipper::intake
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
        servo = hardware_map::get<Servo>("clipper_intake");
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

        log(ANDROID_LOG_INFO, "clipper_intake") << "Setting rotation to: " << rotation << endl;

        const int current_method_count = ++method_count;
        last_target_rotation = target_rotation;
        target_rotation = rotation;

        moving = true;

        servo.set_position(clamp((0.5 + (rotation + OFFSET - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

        const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_target_rotation) / MAX_VELOCITY).Value()));
        if (time_needed == 0ms)
        {
            moving = false;
            if (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished);
            return current_method_count == method_count;
        }

        start_time_rotation = chrono::steady_clock::now();

        task_scheduler::run_after([current_method_count, rotation]
                                  {

                    if (current_method_count != method_count)
                        return;

                    log(ANDROID_LOG_INFO, "clipper_intake") << "finished rotating" << endl;
                    moving = false;
                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished); },
                                  time_needed);

        if (wait_until_done)
            wait_for_movement_finish();

        return current_method_count == method_count;
    }

    bool move_to_lower_intake_position(bool wait_until_done)
    {
        return set_rotation(LOWER_INTAKE_POSITION, wait_until_done);
    }

    bool move_to_upper_intake_position(bool wait_until_done)
    {
        return set_rotation(UPPER_INTAKE_POSITION, wait_until_done);
    }

    bool move_to_storage_position(bool wait_until_done)
    {
        return set_rotation(STORAGE_POSITION, wait_until_done);
    }

    bool move_to_deposit_position(bool wait_until_done)
    {
        return set_rotation(DEPOSIT_POSITION, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }
}