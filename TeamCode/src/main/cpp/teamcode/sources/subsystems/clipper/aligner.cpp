#include "subsystems/clipper/aligner.hpp"

#include <semaphore.h>
#include "Units-master/Maths.h"
#include "task_scheduler.hpp"
#include "subsystems/subsystem_utils.hpp"

using namespace sdk;
using namespace std;

namespace clipper::aligner
{
    int current_clip_count = 0;

    namespace right
    {
        Servo_impl_ex servo;

        Millimetre target_position = 0_mm;
        Millimetre last_target_position = 0_mm;
        bool moving = false;

        int threads_waiting_for_movement_finish = 0;
        sem_t moving_finished;

        int method_count = 0;

        void init()
        {
            servo = hardware_map::get<Servo>("clipper_aligner_right");
            servo.set_pwm_range(servo_type::PWM_RANGE);

            method_count = 0;

            threads_waiting_for_movement_finish = 0;
            sem_init(&moving_finished, 0, 0);
        }

        void stop()
        {
            sem_destroy(&moving_finished);
        }

        bool set_position(Millimetre position, bool wait_until_done)
        {
            position = clamp(position, MIN_POSITION, MAX_POSITION);

            log(ANDROID_LOG_INFO, "clipper_aligner_right") << "Setting position to: " << position << endl;

            const int current_method_count = ++method_count;
            last_target_position = target_position;
            target_position = position;

            moving = true;

            servo.set_position(clamp((0.5 + (position + OFFSET - (MAX_POSITION + MIN_POSITION) / 2) / MM_PER_DEGREE / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(position - last_target_position) / MAX_VELOCITY).Value()));
            if (time_needed == 0ms)
            {
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
                return current_method_count == method_count;
            }

            task_scheduler::run_after([current_method_count]
                                      {

                    if (current_method_count != method_count)
                        return;

                    log(ANDROID_LOG_INFO, "clipper_aligner_right") << "finished rotating" << endl;
                    moving = false;
                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished); },
                                      time_needed);

            if (wait_until_done)
                wait_for_movement_finish();

            return current_method_count == method_count;
        }

        void wait_for_movement_finish()
        {
            subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
        }
    }

    namespace left
    {
        Servo_impl_ex servo;

        Millimetre target_position = 0_mm;
        Millimetre last_target_position = 0_mm;
        bool moving = false;

        int threads_waiting_for_movement_finish = 0;
        sem_t moving_finished;

        int method_count = 0;

        void init()
        {
            servo = hardware_map::get<Servo>("clipper_aligner_left");
            servo.set_pwm_range(servo_type::PWM_RANGE);
            servo.set_direction(Servo::Direction::REVERSE);

            method_count = 0;

            threads_waiting_for_movement_finish = 0;
            sem_init(&moving_finished, 0, 0);
        }

        void stop()
        {
            while (threads_waiting_for_movement_finish > 0)
                sem_post(&moving_finished);

            sem_destroy(&moving_finished);
        }

        bool set_position(Millimetre position, bool wait_until_done)
        {
            position = clamp(position, MIN_POSITION, MAX_POSITION);

            log(ANDROID_LOG_INFO, "clipper_aligner_left") << "Setting position to: " << position << endl;

            const int current_method_count = ++method_count;
            last_target_position = target_position;
            target_position = position;

            moving = true;

            servo.set_position(clamp((0.5 + (position + OFFSET - (MAX_POSITION + MIN_POSITION) / 2) / MM_PER_DEGREE / servo_type::RANGE_OF_MOTION).Value(), 0.0, 1.0));

            const chrono::steady_clock::duration time_needed = chrono::milliseconds(static_cast<int>(Millisecond(abs(position - last_target_position) / MAX_VELOCITY).Value()));
            if (time_needed == 0ms)
            {
                moving = false;
                if (threads_waiting_for_movement_finish > 0)
                    sem_post(&moving_finished);
                return current_method_count == method_count;
            }

            task_scheduler::run_after([current_method_count]
                                      {

                    if (current_method_count != method_count)
                        return;

                    log(ANDROID_LOG_INFO, "clipper_aligner_left") << "finished rotating" << endl;
                    moving = false;
                    if (threads_waiting_for_movement_finish > 0)
                        sem_post(&moving_finished); },
                                      time_needed);

            if (wait_until_done)
                wait_for_movement_finish();

            return current_method_count == method_count;
        }

        void wait_for_movement_finish()
        {
            subsystem_utils::wait_for_movement_finish(moving, threads_waiting_for_movement_finish, moving_finished);
        }
    }

    void init()
    {
        right::init();
        left::init();
    }

    void stop()
    {
        right::stop();
        left::stop();
    }

    bool set_position(Millimetre position, bool wait_until_done)
    {
        aligner::right::set_position(position);
        const int method_count_aligner_right = aligner::right::method_count;
        aligner::left::set_position(position);
        const int method_count_aligner_left = aligner::left::method_count;

        return method_count_aligner_right == right::method_count && method_count_aligner_left == left::method_count;
    }

    bool setup_for_clips(int clips, bool wait_until_done)
    {
        current_clip_count = clips;
        aligner::right::set_position(1_in * ceil(clips / 2.0) - 0.5_in);
        const int method_count_aligner_right = aligner::right::method_count;
        aligner::left::set_position(1_in * floor(clips / 2.0) + 0.5_in);
        const int method_count_aligner_left = aligner::left::method_count;

        if (wait_until_done)
        {
            aligner::right::wait_for_movement_finish();
            aligner::left::wait_for_movement_finish();
        }

        return method_count_aligner_right == aligner::right::method_count && method_count_aligner_left == aligner::left::method_count;
    }

    void wait_for_movement_finish()
    {
        right::wait_for_movement_finish();
        left::wait_for_movement_finish();
    }
}