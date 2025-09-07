#include "subsystems/intake/wrist.hpp"

#include "sdk.hpp"
#include "subsystems/subsystem_utils.hpp"
#include "subsystems/intake/arm.hpp"

using namespace sdk;
using namespace std;

namespace intake::wrist
{
    Servo_impl_ex servo;

    Degree manual_set_target_rotation = 0_deg;
    Degree coaxial_target_rotation = -arm::TRANSFER_ROTATION;
    bool moving = false;

    int threads_waiting_for_movement_finish = 0;
    sem_t moving_finished;

    int method_count = 0;

    void init()
    {
        servo = hardware_map::get<Servo>("intake_wrist");
        servo.set_pwm_range(servo_type::PWM_RANGE);

        method_count = 0;

        threads_waiting_for_movement_finish = 0;
        sem_init(&moving_finished, 0, 0);
    }

    void stop()
    {
        sem_destroy(&moving_finished);
    }

    bool coaxial_mode = true;

    bool rotate(Degree manual_rotation, Degree coaxial_rotation, bool wait_until_done)
    {
        Degree rotation = manual_rotation + coaxial_rotation;

        rotation = clamp(rotation, MIN_ROTATION, MAX_ROTATION);
        log(ANDROID_LOG_INFO, "intake_wrist") << "Setting rotation to manual: " << manual_rotation << " coaxial: " << coaxial_rotation << endl;

        const int current_method_count = ++method_count;
        const Degree last_target_rotation = manual_set_target_rotation + coaxial_target_rotation;
        manual_set_target_rotation = manual_rotation;
        coaxial_target_rotation = coaxial_rotation;

        servo.set_position(clamp((0.5 + (rotation + OFFSET - (MAX_ROTATION + MIN_ROTATION) / 2) / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

        const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(rotation - last_target_rotation) / MAX_VELOCITY).Value()));
        if (time_needed == 0ms)
        {
            moving = false;
            if (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished);
            return current_method_count == method_count;
        }

        moving = true;
        const chrono::steady_clock::time_point start_time = chrono::steady_clock::now();
        task_scheduler::run_after([current_method_count, rotation]
                                  {

                if (current_method_count != method_count)
                    return;

                log(ANDROID_LOG_INFO, "intake_wrist") << "finished rotating" << endl;
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished); },
                                  time_needed);

        if (wait_until_done)
            wait_for_movement_finish();

        return current_method_count == method_count;
    }

    bool update_rotation(Degree coaxial_rotation, bool wait_until_done)
    {
        if (coaxial_mode)
            return rotate(manual_set_target_rotation, coaxial_rotation, wait_until_done);
        else
            return true;
    }

    bool set_rotation(Degree rotation, bool wait_until_done, optional<Degree> coaxial_rotation)
    {
        coaxial_mode = !coaxial_rotation.has_value();
        return rotate(rotation, coaxial_rotation.has_value() ? coaxial_rotation.value() : coaxial_target_rotation, wait_until_done);
    }

    bool rotate_to_transfer(bool wait_until_done)
    {
        return set_rotation(0_deg, wait_until_done);
    }

    void wait_for_movement_finish()
    {
        subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
    }

    bool ready_for_transfer()
    {
        return (manual_set_target_rotation + coaxial_target_rotation) == TRANSFER_ROTATION - arm::TRANSFER_ROTATION && !moving;
    }
}