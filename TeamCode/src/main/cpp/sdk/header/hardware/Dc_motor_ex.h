//
// Created by fnlg on 28.12.2023.
//

#ifndef FTCROBOTCONTROLLER_DC_MOTOR_EX_H
#define FTCROBOTCONTROLLER_DC_MOTOR_EX_H

#include "utils.h"
#include "Dc_motor.h"
#include "PIDF_coefficients.h"

namespace sdk {
    class Dc_motor_ex : public Dc_motor {
    public:
        static jclass jclazz;
        jobject dcMotorEx{};

        Dc_motor_ex() = default;

        Dc_motor_ex(const jobject &dcMotorEx);

        ~Dc_motor_ex();

        Dc_motor_ex &operator=(const Dc_motor_ex &dc_motor_ex);

        Dc_motor_ex &operator=(const jobject &dcMotorEx) override;

        // DcMotorEx

        void set_motor_enable() const;

        void set_motor_disable() const;

        [[nodiscard]] bool is_motor_enabled() const;

        void set_velocity(const double &angular_rate) const;

        [[nodiscard]] double get_velocity() const;

        // Only usable with RUN_USING_ENCODER or RUN_TO_POSITION
        void
        set_PIDF_coefficients(const Run_mode &mode,
                              const PIDF_coefficients &pid_coefficients) const;

        [[nodiscard]] PIDF_coefficients get_PIDF_coefficients(const Run_mode &mode) const;

        void set_target_position_tolerance(const int &tolerance) const;

        [[nodiscard]] int get_target_position_tolerance() const;

        [[nodiscard]] bool is_over_current() const;
    };

} // SDK

#endif //FTCROBOTCONTROLLER_DC_MOTOR_EX_H
