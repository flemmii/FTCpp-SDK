//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_DC_MOTOR_EX_H
#define FTCROBOTCONTROLLER_DC_MOTOR_EX_H

#include <map>

#include "utils.h"
#include "Dc_motor_simple.h"
#include "PIDF_coefficients.h"

namespace sdk {
    extern jclass DcMotorEx;
    extern jclass DcMotor_RunMode;
    extern jclass PIDFCoefficients;
    extern jclass DcMotor_ZeroPowerBehavior;

    class Dc_motor_ex : public Dc_motor_simple {
    public:
        enum class Run_mode {
            RUN_WITHOUT_ENCODER,
            RUN_USING_ENCODER,
            RUN_TO_POSITION,
            STOP_AND_RESET_ENCODER
        };

        enum class Zero_power_behavior {
            UNKNOWN,
            BRAKE,
            FLOAT
        };

        jobject dcMotorEx{};

        Dc_motor_ex() = default;

        Dc_motor_ex(jobject dcMotorEx);

        ~Dc_motor_ex();

        Dc_motor_ex &operator=(jobject dcMotorEx);

        // DcMotorEx

        void set_motor_enable() const;

        void set_motor_disable() const;

        [[nodiscard]] bool is_motor_enabled() const;

        void set_velocity(double angularRate) const;

        [[nodiscard]] double get_velocity() const;

        // Only usable with RUN_USING_ENCODER or RUN_TO_POSITION
        void set_PIDF_coefficients(Run_mode mode, PIDF_coefficients pidCoefficients) const;

        [[nodiscard]] PIDF_coefficients get_PIDF_coefficients(Run_mode mode) const;

        void set_target_position_tolerance(int tolerance) const;

        [[nodiscard]] int get_target_position_tolerance() const;

        [[nodiscard]] bool is_over_current() const;

        // DcMotor
        [[nodiscard]] int get_port_number() const;

        void set_mode(Run_mode mode) const;

        [[nodiscard]] Run_mode get_mode() const;

        static const char *run_mode_to_string(Run_mode mode);

        void set_zero_power_behavior(Zero_power_behavior zero_power_behavior) const;

        [[nodiscard]] Zero_power_behavior get_zero_power_behavior() const;

        static const char *zero_power_behavior_to_string(Zero_power_behavior zero_power_behavior);

        void set_target_position(int position) const;

        [[nodiscard]] int get_target_position() const;

        [[nodiscard]] bool is_busy() const;

        [[nodiscard]] int get_current_position() const;
    };

} // SDK

#endif //FTCROBOTCONTROLLER_DC_MOTOR_EX_H
