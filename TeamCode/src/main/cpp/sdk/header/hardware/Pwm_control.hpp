#pragma once

#include "utils.hpp"

namespace sdk
{
    /**
     * For hardware devices which are manipulated using a pulse width modulation (PWM) signal,
     * the {@link PwmControl} interface provides control of the width of pulses used and whether
     * the PWM is enabled or disabled.
     *
     * <p>PWM is commonly used to control servos. {@link PwmControl} is thus found as a second
     * interface on servo objects whose primary interface is {@link Servo} or {@link CRServo}
     * when the underlying servo controller hardware supports this fine-grained control (not all
     * servo controllers provide such control). To access the {@link PwmControl} interface,
     * cast your {@link Servo} or {@link CRServo} object to {@link PwmControl}; however, it is
     * usually prudent to first test whether the cast will succeed by testing using <pre>instanceof</pre>.
     *
     * @see DcMotorEx
     * @see <a href="https://en.wikipedia.org/wiki/Pulse-width_modulation">Pulse-width modulation</a>
     */
    class Pwm_control
    {
    public:
        /**
         * PwmRange instances are used to specify the upper and lower pulse widths
         * and overall framing rate for a servo.
         *
         * @see <a href="http://www.endurance-rc.com/ppmtut.php">Guide to PWM and PPM</a>
         * @see <a href="https://www.servocity.com/html/hs-485hb_servo.html">HS-485HB servo information</a>
         */
        struct Pwm_range
        {
        public:
            static jclass jclazz;

            /** us_frame_default is the default frame rate used, in microseconds */
            static constexpr double us_frame_default = 20000;
            static constexpr double us_pulse_upper_default = 2400;
            static constexpr double us_pulse_lower_default = 600;

            /** default_range is the default PWM range used */
            static const Pwm_range default_range;

            /** us_pulse_lower is the minimum PWM rate used, in microseconds. This corresponds to a servo position of 0.0. */
            const double us_pulse_lower;
            /** us_pulse_lower is the maximum PWM rate used, in microseconds. This corresponds to a servo position of 1.0. */
            const double us_pulse_upper;
            /** us_frame is the rate, in microseconds, at which the PWM is transmitted. */
            const double us_frame;

            /**
             * Creates a new PwmRange with the indicated lower and upper bounds and the default
             * framing rate.
             * @param us_pulse_lower the minimum PWM rate used, in microsecond
             * @param us_pulse_upper the maximum PWM rate used, in microseconds
             */
            constexpr Pwm_range(const double &us_pulse_lower, const double &us_pulse_upper) : us_pulse_lower(us_pulse_lower), us_pulse_upper(us_pulse_upper), us_frame(us_frame_default) {}
            /**
             * Creates a new PwmRange with the indicated lower and upper bounds and the specified
             * framing rate.
             * @param us_pulse_lower the minimum PWM rate used, in microsecond
             * @param us_pulse_upper the maximum PWM rate used, in microseconds
             * @param us_frame the framing rate, in microseconds
             */
            constexpr Pwm_range(const double &us_pulse_lower, const double &us_pulse_upper, const double &us_frame) : us_pulse_lower(us_pulse_lower), us_pulse_upper(us_pulse_upper), us_frame(us_frame) {}

            /**
             * Equality operator to compare two Pwm_range objects.
             * @param other the Pwm_range object to compare with
             * @return true if all attributes are equal, false otherwise
             */
            constexpr bool operator==(const Pwm_range &other) const
            {
                return us_pulse_lower == other.us_pulse_lower &&
                       us_pulse_upper == other.us_pulse_upper &&
                       us_frame == other.us_frame;
            };
        };

        static jclass jclazz;
        jobject pwmControl{};

        Pwm_control() = default;

        Pwm_control(const jobject &pwm_control);

        ~Pwm_control();

        Pwm_control &operator=(const Pwm_control &pwm_control);

        Pwm_control &operator=(const jobject &pwm_control);

        /**
         * Sets the PWM range limits for the servo
         * @param range the new PWM range limits for the servo
         * @see #get_pwm_range()
         */
        void set_pwm_range(const Pwm_range &range);

        /**
         * Returns the current PWM range limits for the servo
         * @return the current PWM range limits for the servo
         * @see #set_pwm_range(Pwm_range)
         */
        [[nodiscard]] Pwm_range get_pwm_range();

        /**
         * Individually energizes the PWM for this particular servo.
         * @see #set_pwm_disable()
         * @see #is_pwm_enabled()
         */
        void set_pwm_enable();

        /**
         * Individually denergizes the PWM for this particular servo
         * @see #set_pwm_enable()
         */
        void set_pwm_disable();

        /**
         * Returns whether the PWM is energized for this particular servo
         * @see #setPwmEnable()
         */
        [[nodiscard]] bool is_pwm_enabled();
    };
}
