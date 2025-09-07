#pragma once

#include "utils.hpp"

#include "Dc_motor_simple.hpp"

namespace sdk
{
    class Dc_motor : public Dc_motor_simple
    {
    public:
        static jclass jclazz;
        static jclass jclazz_RunMode;
        static jclass jclazz_ZeroPowerBehavior;
        jobject dcMotor{};

        enum class Zero_power_behavior
        {
            UNKNOWN,
            BRAKE,
            FLOAT
        };

        enum class Run_mode
        {
            RUN_WITHOUT_ENCODER,
            RUN_USING_ENCODER,
            RUN_TO_POSITION,
            STOP_AND_RESET_ENCODER
        };

        Dc_motor() = default;

        Dc_motor(const jobject &dcMotor);

        ~Dc_motor();

        Dc_motor &operator=(const Dc_motor &dc_motor);

        Dc_motor &operator=(const jobject &dcMotor) override;

        [[nodiscard]] int get_port_number() const;

        void set_mode(const Run_mode &mode) const;

        [[nodiscard]] Run_mode get_mode() const;

        static const char *run_mode_to_string(const Run_mode &mode);

        void set_zero_power_behavior(const Zero_power_behavior &zero_power_behavior) const;

        [[nodiscard]] Zero_power_behavior get_zero_power_behavior() const;

        static const char *
        zero_power_behavior_to_string(const Zero_power_behavior &zero_power_behavior);

        void set_target_position(const int &position) const;

        [[nodiscard]] int get_target_position() const;

        [[nodiscard]] bool is_busy() const;

        [[nodiscard]] int get_current_position() const;
    };

} // sdk
