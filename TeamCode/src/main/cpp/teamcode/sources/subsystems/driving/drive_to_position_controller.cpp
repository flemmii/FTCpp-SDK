#include "subsystems/driving/drive_to_position_controller.hpp"

#include "sdk.hpp"
#include "utils/PD_controller.hpp"
#include "subsystems/driving/pinpoint.hpp"
#include "match_data.hpp"
#include "subsystems/subsystem_utils.hpp"

using namespace std;
using namespace sdk;

namespace drive_to_position_controller
{
    // ------------------------- THESE SETTINGS CAN BE ADJUSTED --------------------------------

    // _________ General settings _________

    // This has multiple use cases, so you should know where to change this properly
    constexpr Location<Millimetre, Degree> max_deviations(100_mm, 20_deg);

    // Be careful with changing this values cause it effects all background tasks
    constexpr chrono::steady_clock::duration execution_interval = 25ms;

    // Reverses the target x  and rotation when we are on the blue alliance
    constexpr bool reverse_when_blue_alliance = false;

    // Specifies what the maximum motor power can be
    Power max_motor_power = 1_power;

    // _________ Path Generation settings _________

    // Defines roughly how many points per mm are calculated. When using lower values predefining is recommended
    constexpr auto percent_per_loop_multiplier = 2_mm / 1_power;

    // _________ Driving settings _________

    constexpr Millimetre offset_for_coordinates_small_pid = 150_mm;
    constexpr Degree offset_for_turn_small_pid = 45_deg;

    PD_controller pd_coordinates_driving_small(0.002, 0.005);
    PD_controller pd_turn_driving_small(0.03, 0.07);

    PD_controller pd_coordinates_driving_large(0.006, 0.01);
    PD_controller pd_turn_driving_large(0.08, 0.16);

    // _________ Deceleration settings _________

    // Deceleration = (MAX_DECELERATION - DECELERATION_FRICTION) * deceleration_multiplier + DECELERATION_FRICTION.

    // @brief Most of the time it's better to set this lower than 1
    // Physical explanation: When below the maximum deceleration the robot will use the static friction to decelerate
    // and not slip. When above the maximum deceleration the robot will use the kinetic friction which is lower than the
    // static friction and the robot will slip. This case can't happen, because the deceleration is limited by the maximum
    // deceleration, but instead it will just overshoot and correct the position afterwards
    // -> It's better to set this lower than 1, because the robot can handle larger deviations while decelerating, but the
    // deceleration process will take longer
    double deceleration_multiplier = 0.95;

    // Maximum distance from target allowed
    Location<> max_distance_from_target(10_mm, 3_deg);

    // The target velocity when decelerating, when reaching this velocity the robot will stop decelerating and continue to drive
    // with the minimum motor power until the target is reached
    constexpr Speed target_velocity = 400_mm / 1_s;

    // The distance at which power 1 should be applied in the correction phase
    constexpr Millimetre distance_for_max_power = 600_mm;

    // ------------------------- DO NOT CHANGE ANYTHING BELOW THIS LINE -------------------------

    const Location<PD_controller &, PD_controller &> pds_driving_small(pd_coordinates_driving_small,
                                                                       pd_turn_driving_small);
    const Location<PD_controller &, PD_controller &> pds_driving_large(pd_coordinates_driving_large,
                                                                       pd_turn_driving_large);

    Position<> starting_position(0_mm, 0_mm, 0_deg);
    Position<> target_position(0_mm, 0_mm, 0_deg);
    chrono::steady_clock::duration timeout = 0ms;

    bool position_reached = true;

    auto start_time = chrono::steady_clock::now();
    int threads_waiting_until_position_reached = 0;
    sem_t wait_until_position_reached;

    // These variables get their values from the predefine path methode
    vector<vector<Vec2d<>>> control_points_predefined_curves;
    vector<vector<Vec2d<>>> points_on_predefined_curves;
    vector<vector<tuple<Vec2d<>, Vec2d<>>>> speed_points_on_predefined_curves;

    void init()
    {
        target_position = pinpoint::get_position();

        threads_waiting_until_position_reached = 0;
        sem_init(&wait_until_position_reached, 0, 0);
    }

    void stop()
    {
        sem_destroy(&wait_until_position_reached);
    }

    // --------------------- Get Variables -----------------------------

    ::Acceleration get_deceleration()
    {
        return (MAX_DECELERATION - DECELERATION_FRICTION) * deceleration_multiplier + DECELERATION_FRICTION;
    }

    // --------------------- Calculating stop point -----------------------------

    // Splitting the given deceleration into x, y and rotation
    Position<true, ::Acceleration, AngularAcceleration> split_deceleration(const Position<true, Speed, AngularSpeed> &speed)
    {
        const ::Acceleration deceleration = get_deceleration();
        if (speed.is_zero())
            return {deceleration / 4, deceleration / 4, pinpoint::conv_to_rotation<Second2>(deceleration / 2)};

        Location<Speed, Speed> speed_loc(speed.total_distance(), abs(pinpoint::conv_to_distance<Second>(speed.rotation)));
        Location<::Acceleration, ::Acceleration> deceleration_loc(deceleration * (speed_loc.coordinates / (speed_loc.coordinates + speed_loc.rotation)),
                                                                  deceleration * (speed_loc.rotation / (speed_loc.coordinates + speed_loc.rotation)));

        const ::Acceleration deceleration_y = deceleration_loc.coordinates * (abs(speed.y) / speed.total_distance());
        return {
            sqrt(deceleration_loc.coordinates * deceleration_loc.coordinates - deceleration_y * deceleration_y),
            deceleration_y,
            pinpoint::conv_to_rotation<Second2>(deceleration_loc.rotation)};
    }

    // Calculating the point where we would stop with the current speed
    Position<>
    calc_stop_point(const Position<true, ::Acceleration, AngularAcceleration> &deceleration, const Position<true, Speed, AngularSpeed> &speed,
                    const Position<> &position)
    {
        return {subsystem_utils::calc_distance_needed_to_stop(speed.x, deceleration.x) + position.x,
                subsystem_utils::calc_distance_needed_to_stop(speed.y, deceleration.y) + position.y,
                subsystem_utils::calc_distance_needed_to_stop(speed.rotation, deceleration.rotation) + position.rotation};
    }

    // --------------------- Calculating powers -----------------------------

    Power calc_pathing_power_rotation(const Location<> &distance_to_next_bezier_point)
    {
        // This was an idea but it never worked, maybe someone else has a smarter idea but for now I will just return 0 power
        // and the corrective power fixes offsets
        return 0_power;
        return clamp(1_power * pinpoint::conv_to_distance(distance_to_next_bezier_point.rotation) / abs(distance_to_next_bezier_point.coordinates),
                     -max_motor_power, max_motor_power);
    }

    Vec2d<true, Power>
    calc_corrective_power_coordinates(const Vec2d<> &distance_to_target, PD_controller &pd)
    {
        if (distance_to_target.is_zero())
            return {0_power, 0_power};

        const Millimetre distance = distance_to_target.total_distance();
        const Power power_coordinates(pd.calculate_pid_algorithm(distance.Value()));

        return {Power(power_coordinates * (distance_to_target.x / distance)),
                Power(power_coordinates * (distance_to_target.y / distance))};
    }

    Position<true, Power, Power>
    calc_corrective_power(const Position<> &error, Location<PD_controller &, PD_controller &> pds)
    {
        return {calc_corrective_power_coordinates(error.to_vec(), pds.coordinates),
                Power(pds.rotation.calculate_pid_algorithm(error.rotation.Value()))};
    }

    Millimetre estimate_curve_length(const vector<Vec2d<>> &control_points)
    {
        Millimetre length(0);
        for (int i = 0; i < control_points.size() - 1; i++)
        {
            length += (control_points[i + 1] - control_points[i]).total_distance();
        }
        return length;
    }

    static void drive_to_position(const vector<Vec2d<>> &control_points, const double &percent_per_loop, const bool &rotation_should_follow_path, const bool &arrive_forwards = true);
    static void turn_to_rotation();

    // Returns whether it was successful or not
    void decelerate_controlled(const Millimetre &distance_to_target_needed,
                               const chrono::steady_clock::time_point &start_time)
    {
        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "decelerating now" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;

        drivetrain::clip_lower_powers = false;

        // Calculate the distance to the target
        const auto distance_to_target_loc = make_shared<Location<>>(Location<>(target_position - pinpoint::get_position()));
        const auto start_turn_to_rotation = make_shared<bool>(false);
        bool started_correction_phase = false;

        // Runs until we don't move anymore (or some other cases occur)
        task_scheduler::register_task(
            (function<bool()>)[=]() mutable {
                if (distance_to_target_loc->coordinates > distance_to_target_needed ||
                    (chrono::steady_clock::now() - start_time > timeout &&
                     timeout != 0ms))
                    return true;

                // Initialization of our current position and speed
                const Position current_position = pinpoint::get_position();
                log(ANDROID_LOG_DEBUG, "dtpc") << "current position " << current_position << endl;
                const Position speed = pinpoint::get_speed();
                log(ANDROID_LOG_DEBUG, "dtpc") << "speed " << speed << endl;

                // Calculating the distance to the target
                const Position<> distance_to_target = target_position - current_position;
                *distance_to_target_loc = Location<Millimetre, Degree>(distance_to_target);
                log(ANDROID_LOG_DEBUG, "dtpc") << "distance to target " << *distance_to_target_loc << endl;

                if (started_correction_phase ||
                    ((speed.total_distance() < target_velocity || abs(maths::Arctan(speed.y, speed.x) - maths::Arctan(distance_to_target.y, distance_to_target.x)) > 90_deg) &&
                     (abs(speed.rotation) < pinpoint::conv_to_rotation<Second>(target_velocity) || maths::Sgn(speed.rotation) != maths::Sgn(distance_to_target.rotation))))
                {
                    started_correction_phase = true;
                    const Power min_motor_power = drivetrain::update_min_motor_power();

                    const Location power_loc(distance_to_target_loc->coordinates < max_distance_from_target.coordinates ? 0_power : clamp((1_power - min_motor_power) / (distance_for_max_power - max_distance_from_target.coordinates) * (distance_to_target_loc->coordinates - max_distance_from_target.coordinates) + min_motor_power, 0_power, max_motor_power),
                                             abs(distance_to_target_loc->rotation) < max_distance_from_target.rotation ? 0_power : clamp((1_power - min_motor_power) / (pinpoint::conv_to_rotation(distance_for_max_power) - max_distance_from_target.rotation) * (abs(distance_to_target_loc->rotation) - max_distance_from_target.rotation) + min_motor_power, 0_power, max_motor_power));

                    log(ANDROID_LOG_DEBUG, "dtpc") << "power " << power_loc << endl;
                    log(ANDROID_LOG_DEBUG, "dtpc") << "------------------------" << endl;

                    drivetrain::drive<true>({distance_to_target.x / distance_to_target_loc->coordinates * power_loc.coordinates,
                                             distance_to_target.y / distance_to_target_loc->coordinates * power_loc.coordinates,
                                             maths::Sgn(distance_to_target.rotation) * power_loc.rotation});
                    return power_loc.is_zero();
                }
                else
                {
                    const Location<Speed, Speed> speed_loc(speed.total_distance(), abs(pinpoint::conv_to_distance<Second>(speed.rotation)));
                    const Location<::Acceleration, ::Acceleration> deceleration(clamp(::Acceleration(subsystem_utils::calc_deceleration(speed.total_distance(), distance_to_target_loc->coordinates) - DECELERATION_FRICTION * speed_loc.coordinates / (speed_loc.coordinates + speed_loc.rotation)),
                                                                                      -DECELERATION_FRICTION * speed_loc.coordinates / (speed_loc.coordinates + speed_loc.rotation), MAX_DECELERATION * speed_loc.coordinates / (speed_loc.coordinates + speed_loc.rotation)),
                                                                                clamp(::Acceleration(pinpoint::conv_to_distance<Second2>(subsystem_utils::calc_deceleration(speed.rotation, distance_to_target_loc->rotation)) - DECELERATION_FRICTION * speed_loc.rotation / (speed_loc.coordinates + speed_loc.rotation)),
                                                                                      -DECELERATION_FRICTION * speed_loc.rotation / (speed_loc.coordinates + speed_loc.rotation), MAX_DECELERATION * speed_loc.rotation / (speed_loc.coordinates + speed_loc.rotation)));
                    log(ANDROID_LOG_DEBUG, "dtpc") << "deceleration " << deceleration << endl;

                    const Position<true, Newton, Newton> force(-distance_to_target.x / distance_to_target_loc->coordinates * deceleration.coordinates * robot::MASS,
                                                               -distance_to_target.y / distance_to_target_loc->coordinates * deceleration.coordinates * robot::MASS,
                                                               -maths::Sgn(distance_to_target.rotation) * deceleration.rotation * robot::MASS);

                    const drivetrain::Drive_motors<Power> power(drivetrain::apply_force_to_robot(force, {0_m / 1_s, 0_m / 1_s, 0_m / 1_s, 0_m / 1_s}));
                    log(ANDROID_LOG_DEBUG, "dtpc") << "power " << drivetrain::conv_to_pos(power) << endl;
                    log(ANDROID_LOG_DEBUG, "dtpc") << "------------------------" << endl;

                    drivetrain::drive(power);
                    return false;
                }
            },
            execution_interval,
            true,
            [=]()
            {
                if (chrono::steady_clock::now() - start_time >= timeout && timeout != 0ms)
                {
                    if (timeout == -1ms)
                        log(ANDROID_LOG_WARN, "decelerate_controlled") << "Ended from outside" << endl;
                    else
                        log(ANDROID_LOG_WARN, "decelerate_controlled") << "Ended due to timeout" << endl;
                }
                // If the upper loop ended due to distancing from the target it will call the driving again
                // Probably we got pushed during deceleration
                else if (distance_to_target_loc->coordinates > distance_to_target_needed)
                {
                    log(ANDROID_LOG_WARN, "decelerate_controlled") << "restarting driving operation" << endl;

                    Vec2d current_coordinates = pinpoint::get_position().to_vec();
                    vector<Vec2d<>> control_points{current_coordinates, target_position.to_vec()};

                    // Adjusting the percent per loop to how fast we'll drive and how far we'll drive
                    // What this changes is explained at the percent_per_loop_multiplier declaration
                    const double percent_per_loop = percent_per_loop_multiplier *
                                                    (max_motor_power / estimate_curve_length(control_points));

                    // If we just need to turn the normal drive to position won't work, that's why there is a custom turn
                    // to rotation for such a case
                    if ((starting_position - target_position).total_distance() > max_deviations.coordinates)
                        drive_to_position(control_points, percent_per_loop, false);
                    else
                        turn_to_rotation();

                    return;
                }

                drivetrain::set_zero_power_behavior(Dc_motor_ex::Zero_power_behavior::BRAKE);
                drivetrain::drive<false>({0_power, 0_power, 0_power});
                position_reached = true;
                log(ANDROID_LOG_INFO, "decelerate_controlled") << "decelerating ended" << endl;

                if (threads_waiting_until_position_reached > 0)
                    sem_post(&wait_until_position_reached);
            });
    }

    // --------------------- Common rotation functions -----------------------------

    Degree calc_tangent_angle(const tuple<Vec2d<>, Vec2d<>> &speed_points)
    {
        const Vec2d speed = get<1>(speed_points) - get<0>(speed_points);
        return Radian(atan2(speed.x.Value(), speed.y.Value()));
    }

    Degree conv_to_continuous(const Degree &angle, const Degree &current_rotation)
    {
        return round((current_rotation - angle) / 360_deg) * 360_deg + angle;
    }

    Degree calc_abs_target_rotation(const Degree &current_rotation,
                                    const Degree &tangent_angle,
                                    const bool &arrive_forwards = true)
    {
        // Calculating the final target rotation
        Degree rel_target_rotation = tangent_angle - 180_deg * !arrive_forwards;
        return conv_to_continuous(rel_target_rotation, current_rotation);
    }

    // --------------------- Bezier curves -----------------------------

    tuple<Vec2d<>, tuple<Vec2d<>, Vec2d<>>>
    get_point_on_curve(const vector<Vec2d<>> &control_points, const double &percent)
    {
        vector<Vec2d<>> points = control_points;
        vector<Vec2d<>> speed_points;

        // looping until only one point is left -> This is our point on the bezier curve
        while (points.size() > 1)
        {
            // When there are only two points remaining, these two points are the speed points.
            // A vector between them is the tangent of the bezier curve in the currently calculating point
            if (points.size() == 2)
            {
                speed_points = points;
            }

            // Vector to store the points calculated after this
            vector<Vec2d<>> new_points;
            new_points.reserve(points.size() - 1);

            // Calculating all new points -> The new points size is the points size - 1
            for (int i = 0; i < points.size() - 1; i++)
            {
                new_points.emplace_back(points[i].x + ((points[i + 1].x - points[i].x) * percent),
                                        points[i].y + ((points[i + 1].y - points[i].y) * percent));
            }
            points = new_points;
        }

        return {points.front(), make_tuple(speed_points[0], speed_points[1])};
    }

    bool bezier_point_gets_further_from_cur_pos(const Vec2d<> &coordinates,
                                                const Vec2d<> &next_bezier_point,
                                                const Vec2d<> &bezier_point)
    {
        Vec2d difference = next_bezier_point - bezier_point;
        if (difference.y == 0_mm)
        {
            // Wenn die y-Koordinate gleich 0 ist, ist die Gerade horizontal
            // In diesem Fall ist der Punkt P der nächste Punkt auf der Geraden
            return (next_bezier_point.x > bezier_point.x) == (coordinates.x > bezier_point.x);
        }

        // Berechne die Steigung der Geraden
        const double slope = -difference.x / difference.y;

        // Berechne f(x, y) = y - (mx + b) für beide Punkte
        Millimetre y_next_bezier_point_on_normal = slope * difference.x + bezier_point.y;
        Millimetre y_coordinates_on_normal = slope * (coordinates.x - bezier_point.x) + bezier_point.y;

        // Überprüfe, ob beide Werte dasselbe Vorzeichen haben
        return (next_bezier_point.y > y_next_bezier_point_on_normal) == (coordinates.y > y_coordinates_on_normal);
    }

    // --------------------- Decelerating condition -----------------------------

    void get_closest_point_to_robot(double &current_percent, const double &percent_per_loop,
                                    const Vec2d<> &current_coordinates,
                                    const vector<Vec2d<>> &control_points,
                                    Vec2d<> &next_bezier_point,
                                    tuple<Vec2d<>, Vec2d<>> &next_speed_points,
                                    Vec2d<> &bezier_point,
                                    tuple<Vec2d<>, Vec2d<>> &speed_points)
    {
        while ((current_percent + percent_per_loop) < 1 &&
               bezier_point_gets_further_from_cur_pos(current_coordinates, next_bezier_point,
                                                      bezier_point))
        {
            // raising the current percent every loop so the point on the bezier can advance
            current_percent += percent_per_loop;

            bezier_point = next_bezier_point;
            speed_points = next_speed_points;

            // If the distance between our robot and the bezier point gets greater, we know our last point was the closest
            // to our robot and this point is also in front of our robot, so the robot won't drive backwards.
            // If this is not the case we store our current point and speed points and do another loop

            const tuple cache = get_point_on_curve(control_points, current_percent);
            next_bezier_point = get<0>(cache);
            next_speed_points = get<1>(cache);
        }
    }

    bool close_enough_to_target(const Position<> &position,
                                const Position<true, Speed, AngularSpeed> &velocity,
                                const Vec2d<> &position_on_bezier)
    {
        // Breaks this loop if the distance to the target gets low enough to decelerate
        const Position deceleration = split_deceleration(velocity);

        const Vec2d stop_point = calc_stop_point(deceleration, velocity, position).to_vec();
        log(ANDROID_LOG_DEBUG, "dtpc") << "calc stop point " << stop_point << endl;

        Vec2d difference = position_on_bezier - target_position.to_vec();
        if (difference.y == 0_mm)
        {
            // Wenn die y-Koordinate gleich 0 ist, ist die Gerade horizontal
            // In diesem Fall ist der Punkt P der nächste Punkt auf der Geraden
            return (position_on_bezier.x > target_position.x) != (stop_point.x > target_position.x);
        }

        // Berechne die Steigung der Geraden
        const double slope = -difference.x / difference.y;

        // Berechne f(x, y) = y - (mx + b) für beide Punkte
        Millimetre y_position_on_bezier_point_on_normal = slope * difference.x + target_position.y;
        Millimetre y_stop_point_on_normal = slope * (stop_point.x - target_position.x) + target_position.y;

        // Überprüfe, ob beide Werte dasselbe Vorzeichen haben
        return (position_on_bezier.y > y_position_on_bezier_point_on_normal) != (stop_point.y > y_stop_point_on_normal);
    }

    // --------------------- Calculating the powers -----------------------------

    // Total speed calculation. The higher the rank in this array the higher the priority. Also in the speeds
    // rotation has a higher priority than direction speed
    Position<false, Power, Power> add_up_powers_based_on_hierarchy(const vector<Position<false, Power, Power>> &powers)
    {
        Position<false, Power, Power> total_power(0_power, 0_power, 0_power);
        for (size_t i = 0; i < powers.size(); ++i)
        {
            Power power_remaining = max_motor_power -
                                    (abs(total_power.forwards) + abs(total_power.sidewards) +
                                     abs(total_power.rotation));

            Position power = powers[i];

            if (i == powers.size() - 1)
            {
                // Rotational power does NOT have a higher priority than coordinatel power
                const double percent_of_power = power_remaining / (abs(power.forwards) + abs(power.sidewards) + abs(power.rotation));

                if (percent_of_power > 1)
                {
                    total_power += power;
                }
                else
                {
                    total_power += power * percent_of_power;
                    break;
                }
            }
            else
            {
                // Rotational power has a higher priority than coordinatel power
                if (power.rotation != 0_power)
                {
                    const double percent_of_power = power_remaining / abs(power.rotation);

                    if (percent_of_power > 1)
                    {
                        total_power.rotation += power.rotation;
                    }
                    else
                    {
                        total_power.rotation += power.rotation * percent_of_power;
                        break;
                    }
                    power_remaining -= abs(power.rotation);
                }

                if (power.forwards != 0_power || power.sidewards != 0_power)
                {
                    const double percent_of_power =
                        power_remaining / (abs(power.forwards) + abs(power.sidewards));

                    const Vec2d<false, Power> direction_power(power.forwards, power.sidewards);

                    if (percent_of_power > 1)
                    {
                        total_power += direction_power;
                    }
                    else
                    {
                        total_power += direction_power * percent_of_power;
                        break;
                    }
                }
            }
        }
        return total_power;
    }

    Vec2d<true, Power> conv_to_motor_power(const Vec2d<> &distance_speed_points)
    {
        Vec2d<true, Power> power(0_power, 0_power);
        if (distance_speed_points.x != 0_mm || distance_speed_points.y != 0_mm)
        {
            if (abs(distance_speed_points.x) > abs(distance_speed_points.y))
            {
                power.y = 1_power * distance_speed_points.y / abs(distance_speed_points.x);
                power.x = 1_power * maths::Sgn(distance_speed_points.x);
            }
            else
            {
                power.x = 1_power * distance_speed_points.x / abs(distance_speed_points.y);
                power.y = 1_power * maths::Sgn(distance_speed_points.y);
            }
        }
        return power;
    }

    // Centripetal force/acceleration calculation to stay on circular path (This equation got simplified a lot)
    drivetrain::Drive_motors<Power>
    calc_centripetal_force(Millimetre distance, const Degree &angle,
                           const Degree &would_be_next_angle,
                           Vec2d<true, Speed> speed)
    {
        drivetrain::Drive_motors<Power> centripetal_force_power(0_power, 0_power, 0_power, 0_power);

        Degree angle_difference = would_be_next_angle - angle;
        if (angle_difference > 180_deg)
            angle_difference -= 360_deg;
        else if (angle_difference < -180_deg)
            angle_difference += 360_deg;

        if (angle_difference != 0_deg && distance != 0_mm)
        {
            const Millimetre radius = sqrt(distance * distance /
                                           (2 * (1 - maths::Cos(angle_difference))));
            const Newton centripetal_force = robot::MASS * (speed.x * speed.x + speed.y * speed.y) / radius;

            const Degree rotation_of_centripetal_force =
                angle + 90_deg * maths::Sgn(angle_difference);
            const Vec2d centripetal_force_vector = pinpoint::conv_to_robot_centric<Newton>(
                {centripetal_force * maths::Sin(rotation_of_centripetal_force),
                 centripetal_force * maths::Cos(rotation_of_centripetal_force)});

            centripetal_force_power = drivetrain::apply_force_to_robot(Position<false, Newton, Newton>(centripetal_force_vector, 0_N), drivetrain::get_motor_speeds());
        }
        return centripetal_force_power;
    }

    // If our distance to the target is greater than the offset we use a small pid instead of a large one
    Location<PD_controller &, PD_controller &>
    get_pds_depending_on_dissent(const Position<> &dissent)
    {
        Location<PD_controller *, PD_controller *> pds_driving(nullptr, nullptr);

        if (dissent.total_distance() > offset_for_coordinates_small_pid)
            pds_driving.coordinates = &pds_driving_small.coordinates;
        else
            pds_driving.coordinates = &pds_driving_large.coordinates;

        if (abs(dissent.rotation) > offset_for_turn_small_pid)
            pds_driving.rotation = &pds_driving_small.rotation;
        else
            pds_driving.rotation = &pds_driving_large.rotation;

        return {*pds_driving.coordinates, *pds_driving.rotation};
    }

    Position<false, Power, Power>
    calc_total_power(const Vec2d<> &next_bezier_point, const Vec2d<> &bezier_point,
                     const Degree &current_tangent_angle,
                     const Degree &next_tangent_angle,
                     const Degree &current_target_rotation,
                     const Degree &delta_rotation,
                     const Position<> &current_position,
                     const Vec2d<true, Speed> &current_velocity,
                     const Vec2d<true, Power> &pathing_power_vec)
    {
        // Pathing speed calculation: The higher pathing speed is always 1 or -1, the other
        // speed coordinate is in relation to the higher speed
        const Position<true, Power, Power> pathing_power_field_centric =
            {pathing_power_vec, calc_pathing_power_rotation({(next_bezier_point - bezier_point).total_distance(), delta_rotation})};
        const Position<false, Power, Power> pathing_power(pinpoint::conv_to_robot_centric(pathing_power_field_centric.to_vec()),
                                                          pathing_power_field_centric.rotation);
        log(ANDROID_LOG_DEBUG, "dtpc") << "pathing power " << pathing_power << endl;

        const Position<> position_on_bezier(bezier_point.x, bezier_point.y,
                                            current_target_rotation);
        log(ANDROID_LOG_DEBUG, "dtpc") << "position on bezier " << position_on_bezier << endl;

        const Position<false, Power, Power> centripetal_power = drivetrain::conv_to_pos(calc_centripetal_force(
            (next_bezier_point - bezier_point).total_distance(), current_tangent_angle,
            next_tangent_angle, current_velocity));
        log(ANDROID_LOG_DEBUG, "dtpc") << "centripetal force " << centripetal_power << endl;

        // Corrective speed calculation based on our current position and the optimal position
        const Position<> dissent = position_on_bezier - current_position;
        const Position corrective_power = calc_corrective_power(dissent,
                                                                get_pds_depending_on_dissent(
                                                                    dissent));
        log(ANDROID_LOG_DEBUG, "dtpc") << "corrective power " << corrective_power << endl;
        const Position<false, Power, Power> corrective_power_robot_centric = {
            pinpoint::conv_to_robot_centric(corrective_power.to_vec()),
            corrective_power.rotation};

        Position total_power = pathing_power + corrective_power_robot_centric + centripetal_power;
        const Power max_power = max({abs(total_power.forwards), abs(total_power.sidewards), abs(total_power.rotation)});
        if (max_power > max_motor_power)
        {
            const double scale_factor = max_motor_power / max_power;
            total_power.forwards *= scale_factor;
            total_power.sidewards *= scale_factor;
            total_power.rotation *= scale_factor;
        }

        return total_power;
    }

    Vec2d<true, Power>
    calc_power_coordinates_and_tangent_radians(const tuple<Vec2d<>, Vec2d<>> &next_speed_points,
                                               const tuple<Vec2d<>, Vec2d<>> &speed_points,
                                               Degree &current_tangent_angle,
                                               Degree &next_tangent_angle)
    {
        Vec2d power = conv_to_motor_power(get<1>(speed_points) - get<0>(speed_points));
        Vec2d would_be_next_power = conv_to_motor_power(get<1>(next_speed_points) - get<0>(next_speed_points));

        // Tangent calculation: The tangent to our estimated position on the bezier curve
        current_tangent_angle = maths::Arctan(power.x, power.y);

        // Tangent calculation: The tangent to the next estimated position on the bezier curve
        next_tangent_angle = maths::Arctan(would_be_next_power.x, would_be_next_power.y);

        // Converting the abstract speed vectors to motor power vectors
        return power;
    }

    void calc_target_rotation(Degree &current_target_rotation,
                              Degree &delta_rotation,
                              const Degree &current_rotation,
                              const Degree &current_tangent_angle,
                              const Degree &next_tangent_angle,
                              const double &current_percent,
                              const double &percent_per_loop,
                              const bool &rotation_should_follow_path,
                              const bool &arrive_forwards = true)
    {
        // Target rotation and rotation speed calculation:
        // If we should follow the path with our rotation, the rotation target is just the tangent degrees
        // and the rotation speed is the difference between our current target and our next target.
        // If we don't follow the path with our rotation, the rotation target is just our delta rotation
        // multiplied with the current percent and the rotation_speed is constant the delta rotation
        // multiplied with the percent per loop
        if (rotation_should_follow_path)
        {
            const Degree rel_current_target_rotation = current_tangent_angle - 180_deg * !arrive_forwards;
            current_target_rotation = conv_to_continuous(rel_current_target_rotation, current_rotation);

            const Degree rel_would_be_next_target_rotation = next_tangent_angle - 180_deg * !arrive_forwards;
            const Degree would_be_next_target_rotation = conv_to_continuous(rel_would_be_next_target_rotation, current_rotation);

            delta_rotation = would_be_next_target_rotation - current_target_rotation;
        }
        else
        {
            current_target_rotation = starting_position.rotation +
                                      (target_position.rotation -
                                       starting_position.rotation) *
                                          current_percent;

            delta_rotation = (target_position.rotation - starting_position.rotation) * percent_per_loop;
        }
    }

    // This handles all movements except movements with only rotation
    static void
    drive_to_position(const vector<Vec2d<>> &control_points, const double &percent_per_loop,
                      const bool &rotation_should_follow_path, const bool &arrive_forwards)
    {
        // Resetting the PIDs to ensure they are not influenced by the last driving
        pds_driving_small.coordinates.reset();
        pds_driving_small.rotation.reset();
        pds_driving_large.coordinates.reset();
        pds_driving_large.rotation.reset();

        // Sets all the support systems to the necessary options for this driving algorithm to work
        drivetrain::preset_for_auto();

        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "driving to position now" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;

        log(ANDROID_LOG_DEBUG, "dtpc") << "target position: " << target_position << endl;
        double current_percent = percent_per_loop;

        const auto cache = get_point_on_curve(control_points, current_percent);
        Vec2d<> bezier_point = control_points[0];
        Vec2d<> next_bezier_point = get<0>(cache);
        tuple<Vec2d<>, Vec2d<>> speed_points = {control_points[0],
                                                control_points[1]};
        tuple<Vec2d<>, Vec2d<>> next_speed_points = get<1>(cache);

        // This loop drives along the bezier curve until a certain distance to the target is reached
        task_scheduler::register_task(
            (function<bool()>)[=]() mutable {
                if (chrono::steady_clock::now() - start_time > timeout &&
                    timeout != 0ms)
                {
                    log(ANDROID_LOG_DEBUG, "dtpc") << "driving to position ended due to timeout" << endl;
                    return true;
                }

                // Initialization of our current position, current position is only set here
                const Position current_position = pinpoint::get_position();
                const Position current_velocity = pinpoint::get_speed();
                const Vec2d current_coordinates = current_position.to_vec();

                log(ANDROID_LOG_DEBUG, "dtpc") << "current position " << current_position << endl;
                log(ANDROID_LOG_DEBUG, "dtpc") << "current velocity " << current_velocity << endl;

                get_closest_point_to_robot(current_percent, percent_per_loop,
                                           current_coordinates,
                                           control_points, next_bezier_point, next_speed_points,
                                           bezier_point, speed_points);

                // Breaks this loop if the distance to the target gets low enough to decelerate
                if (close_enough_to_target(current_position, current_velocity, bezier_point))
                    return true;

                Degree current_tangent_angle;
                Degree next_tangent_angle;

                const Vec2d power_coordinates = calc_power_coordinates_and_tangent_radians(
                    next_speed_points, speed_points, current_tangent_angle,
                    next_tangent_angle);

                // Declaration of some variables needed later on
                Degree current_target_rotation;
                Degree delta_rotation;

                calc_target_rotation(current_target_rotation,
                                     delta_rotation,
                                     current_position.rotation,
                                     current_tangent_angle,
                                     next_tangent_angle,
                                     current_percent, percent_per_loop,
                                     rotation_should_follow_path,
                                     arrive_forwards);

                const Position total_power = calc_total_power(next_bezier_point, bezier_point,
                                                              current_tangent_angle,
                                                              next_tangent_angle,
                                                              current_target_rotation,
                                                              delta_rotation,
                                                              current_position,
                                                              current_velocity.to_vec(),
                                                              power_coordinates);

                log(ANDROID_LOG_DEBUG, "dtpc") << "total speed " << total_power << endl;

                drivetrain::drive(total_power);
                log(ANDROID_LOG_DEBUG, "dtpc") << "---------------" << endl;

                return false;
            },
            execution_interval,
            true,
            [=]()
            {
                if (rotation_should_follow_path)
                {
                    target_position.rotation = calc_abs_target_rotation(
                        pinpoint::get_position().rotation, calc_tangent_angle(make_tuple(control_points[control_points.size() - 2], control_points.back())), arrive_forwards);
                }

                decelerate_controlled((target_position - pinpoint::get_position()).total_distance() + 100_mm, start_time);
            });
    }

    void get_closest_point_to_robot(int &current_index, const Vec2d<> &current_coordinates,
                                    const vector<Vec2d<>> &points_on_bezier,
                                    const vector<tuple<Vec2d<>, Vec2d<>>> &speed_points_on_bezier,
                                    Vec2d<> &next_bezier_point,
                                    tuple<Vec2d<>, Vec2d<>> &next_speed_points,
                                    Vec2d<> &bezier_point,
                                    tuple<Vec2d<>, Vec2d<>> &speed_points)
    {
        while ((current_index + 1) < points_on_bezier.size() &&
               bezier_point_gets_further_from_cur_pos(current_coordinates, next_bezier_point,
                                                      bezier_point))
        {
            current_index++;

            bezier_point = next_bezier_point;
            speed_points = next_speed_points;

            next_bezier_point = points_on_bezier.at(current_index);
            next_speed_points = speed_points_on_bezier.at(current_index);
        }
    }

    void calc_target_rotation(Degree &current_target_rotation,
                              Degree &delta_rotation,
                              const Degree &current_rotation,
                              const Degree &current_tangent_angle,
                              const Degree &next_tangent_angle,
                              const double &current_index,
                              const vector<Vec2d<>> &points_on_bezier,
                              const double &percent_per_loop,
                              const bool &rotation_should_follow_path,
                              const bool &arrive_forwards = true)
    {
        calc_target_rotation(current_target_rotation, delta_rotation, current_rotation,
                             current_tangent_angle, next_tangent_angle,
                             (current_index + 1) /
                                 static_cast<double>(points_on_bezier.size()),
                             percent_per_loop, rotation_should_follow_path,
                             arrive_forwards);
    }

    void drive_along_path(const size_t &index_of_predefined_curve, const double &percent_per_loop,
                          const bool &rotation_should_follow_path,
                          const bool &arrive_forwards)
    {
        // Resetting the PIDs to ensure they are not influenced by the last driving
        pds_driving_small.coordinates.reset();
        pds_driving_small.rotation.reset();
        pds_driving_large.coordinates.reset();
        pds_driving_large.rotation.reset();

        // Sets all the support systems to the necessary options for this driving algorithm to work
        drivetrain::preset_for_auto();

        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "driving along path now" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;

        log(ANDROID_LOG_DEBUG, "dtpc") << "index of predefined curve: " << index_of_predefined_curve << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "target position: " << target_position << endl;

        // This gets all pre-calculated points on the bezier curve and the speed points on the bezier curve
        const vector<Vec2d<>> &points_on_bezier = points_on_predefined_curves[index_of_predefined_curve];
        const vector<tuple<Vec2d<>, Vec2d<>>> &speed_points_on_bezier = speed_points_on_predefined_curves[index_of_predefined_curve];

        int current_index = 1;

        Vec2d<> bezier_point = points_on_bezier.front();
        Vec2d<> next_bezier_point = points_on_bezier[current_index];
        tuple<Vec2d<>, Vec2d<>> speed_points = speed_points_on_bezier.front();
        tuple<Vec2d<>, Vec2d<>> next_speed_points = speed_points_on_bezier[current_index];

        // This loop drives along the bezier curve until a certain distance to the target is reached
        task_scheduler::register_task(
            (function<bool()>)[=]() mutable {
                if (chrono::steady_clock::now() - start_time > timeout &&
                    timeout != 0ms)
                {
                    log(ANDROID_LOG_DEBUG, "dtpc") << "driving along path ended due to timeout" << endl;
                    return true;
                }

                // Initialization of our current position, current position is only set here
                const Position current_position = pinpoint::get_position();
                const Position current_velocity = pinpoint::get_speed();
                const Vec2d current_coordinates = current_position.to_vec();

                log(ANDROID_LOG_DEBUG, "dtpc") << "current position " << current_position << endl;
                log(ANDROID_LOG_DEBUG, "dtpc") << "current velocity " << current_velocity << endl;

                get_closest_point_to_robot(current_index, current_coordinates, points_on_bezier,
                                           speed_points_on_bezier, next_bezier_point, next_speed_points,
                                           bezier_point, speed_points);

                // Breaks this loop if the distance to the target gets low enough to decelerate
                if (close_enough_to_target(current_position, current_velocity, bezier_point))
                    return true;

                Degree current_tangent_angle;
                Degree next_tangent_angle;
                const Vec2d power_coordinates = calc_power_coordinates_and_tangent_radians(
                    next_speed_points, speed_points, current_tangent_angle,
                    next_tangent_angle);

                // Declaration of some variables needed later on
                Degree current_target_rotation;
                Degree delta_rotation;

                calc_target_rotation(current_target_rotation,
                                     delta_rotation,
                                     current_position.rotation,
                                     current_tangent_angle,
                                     next_tangent_angle,
                                     current_index, points_on_bezier,
                                     percent_per_loop,
                                     rotation_should_follow_path,
                                     arrive_forwards);

                // Total speed calculation. The higher the rank in this array the higher the priority. Also in the speeds
                // rotation has a higher priority than direction speed
                const Position total_power = calc_total_power(next_bezier_point, bezier_point,
                                                              current_tangent_angle,
                                                              next_tangent_angle,
                                                              current_target_rotation,
                                                              delta_rotation,
                                                              current_position,
                                                              current_velocity.to_vec(),
                                                              power_coordinates);

                log(ANDROID_LOG_DEBUG, "dtpc") << "total power " << total_power << endl;

                drivetrain::drive(total_power);
                log(ANDROID_LOG_DEBUG, "dtpc") << "---------------" << endl;

                return false;
            },
            execution_interval,
            true,
            [=]()
            {
                if (rotation_should_follow_path)
                {
                    target_position.rotation = calc_abs_target_rotation(
                        pinpoint::get_position().rotation,
                        calc_tangent_angle(
                            speed_points_on_predefined_curves[index_of_predefined_curve].back()),
                        arrive_forwards);
                }

                decelerate_controlled((target_position - pinpoint::get_position()).total_distance() + 100_mm, start_time);
            });
    }

    // This turns to a rotation if there is no (large) movement on the x and y axis
    static void turn_to_rotation()
    {
        // Resetting the PIDs to ensure they are not influenced by the last driving
        pds_driving_small.coordinates.reset();
        pds_driving_small.rotation.reset();
        pds_driving_large.coordinates.reset();
        pds_driving_large.rotation.reset();

        // Sets all the support systems to the necessary options for this driving algorithm to work
        drivetrain::preset_for_auto();

        const Degree original_rotation_to_target = target_position.rotation - pinpoint::get_position().rotation;
        shared_ptr<Position<>> distance_to_target = make_shared<Position<>>(Position<>(0_mm, 0_mm, 0_deg));

        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "turning to rotation now" << endl;
        log(ANDROID_LOG_DEBUG, "dtpc") << "#########################################################" << endl;

        task_scheduler::register_task((function<bool()>)[ original_rotation_to_target, distance_to_target ] {
            if (chrono::steady_clock::now() - start_time > timeout &&
                    timeout != 0ms)
                {
                    log(ANDROID_LOG_DEBUG, "dtpc") << "turn to rotation ended due to timeout" << endl;
                    return true;
                }

            const Position current_position = pinpoint::get_position();

            // Breaks this loop if the rotation to the target gets low enough to decelerate
            const AngularSpeed current_speed = pinpoint::get_speed().rotation;

            *distance_to_target = target_position - current_position;
            log(ANDROID_LOG_DEBUG, "dtpc") << "distance to target " << *distance_to_target << endl;

            // Every speed is just handled as a corrective speed except the rotation
            const Position<true, Power, Power> power(calc_corrective_power_coordinates(distance_to_target->to_vec(), get_pds_depending_on_dissent(*distance_to_target).coordinates), 1_power * maths::Sgn(original_rotation_to_target));

            if (subsystem_utils::calc_distance_needed_to_stop(current_speed, pinpoint::conv_to_rotation<Second2>(get_deceleration())) * maths::Sgn(original_rotation_to_target) > distance_to_target->rotation * maths::Sgn(original_rotation_to_target))
                return true;

            drivetrain::drive(power);
            log(ANDROID_LOG_DEBUG, "dtpc") << "---------------" << endl;

            return false; },
                                      execution_interval,
                                      true,
                                      [distance_to_target]
                                      { decelerate_controlled(distance_to_target->total_distance() + 100_mm, start_time); });
    }

    void hold_position()
    {
        task_scheduler::register_task((function<bool()>)[] {
            if (chrono::steady_clock::now() - start_time > timeout && timeout != 0ms)
                return true;

            const Position current_position = pinpoint::get_position();
            log(ANDROID_LOG_DEBUG, "dtpc") << "current position " << current_position << endl;

            const Position<> distance_to_target = target_position - current_position;

            //TODO: Implement driving to position if we get to far away from the target position

            // Every speed is just handled as a corrective speed
            const Position power = calc_corrective_power(distance_to_target, pds_driving_large);

            drivetrain::drive(power);

            return false; }, execution_interval, true, []
                                      {
            if (chrono::steady_clock::now() - start_time > timeout) {
                if (timeout == -1ms)
                    log(ANDROID_LOG_INFO, "hold_position") << "Ended from outside" << endl;
                else
                    log(ANDROID_LOG_INFO, "hold_position") << "Ended due to timeout" << endl;
            }

            position_reached = true;

            if (threads_waiting_until_position_reached > 0)
                sem_post(&wait_until_position_reached); });
    }

    // ----------------- Common functions for predefining the path and on the fly path calculations -----------------

    void reverse_if_blue_alliance(vector<Vec2d<>> &control_points)
    {
        if constexpr (!reverse_when_blue_alliance)
            return;
        if (match_data::current_match::red_alliance)
            return;

        for (Vec2d<> &control_point : control_points)
            control_point.x = -control_point.x;
    }

    void reverse_if_blue_alliance(Position<> &target_position)
    {
        if constexpr (!reverse_when_blue_alliance)
            return;
        if (match_data::current_match::red_alliance)
            return;

        target_position.x = -target_position.x;
        target_position.rotation = -target_position.rotation;
    }

    void reverse_if_blue_alliance(Degree &target_rotation)
    {
        if constexpr (!reverse_when_blue_alliance)
            return;
        if (match_data::current_match::red_alliance)
            return;

        target_rotation = -target_rotation;
    }

    void reverse_if_blue_alliance(Position<> &target_position, vector<Vec2d<>> &control_points)
    {
        if constexpr (!reverse_when_blue_alliance)
            return;
        if (match_data::current_match::red_alliance)
            return;

        target_position.x = -target_position.x;
        target_position.rotation = -target_position.rotation;

        for (Vec2d<> &control_point : control_points)
            control_point.x = -control_point.x;
    }

    size_t
    predefine_path(vector<Vec2d<>> control_points)
    {
        if (control_points.size() < 2)
        {
            log(ANDROID_LOG_ERROR, "dtpc") << "Min 2 control points are needed" << endl;
            return -1;
        }

        size_t index = control_points_predefined_curves.size() + 1;
        control_points_predefined_curves.resize(index);
        vector<Vec2d<>> &control_points_predefined_curve = control_points_predefined_curves.back();

        points_on_predefined_curves.resize(index);
        vector<Vec2d<>> &points_on_predefined_curve = points_on_predefined_curves.back();

        speed_points_on_predefined_curves.resize(index);
        vector<tuple<Vec2d<>, Vec2d<>>> &speed_points_on_predefined_curve = speed_points_on_predefined_curves.back();

        // This sets all control points
        reverse_if_blue_alliance(control_points);
        control_points_predefined_curve = control_points;

        // This is to adjust the percent per loop to how fast we'll drive and how far we'll drive
        const double percent_per_loop =
            percent_per_loop_multiplier *
            (max_motor_power / estimate_curve_length(control_points));

        double current_percent = 0;
        while (current_percent < 1)
        {
            const tuple cache = get_point_on_curve(control_points, current_percent);
            points_on_predefined_curve.push_back(get<0>(cache));
            speed_points_on_predefined_curve.push_back(get<1>(cache));

            // raising the current percent every loop so the point on the bezier can advance
            current_percent += percent_per_loop;
        }
        points_on_predefined_curve.push_back(control_points.back());
        speed_points_on_predefined_curve.emplace_back(control_points[control_points.size() - 2],
                                                      control_points.back());

        return index;
    }

    vector<size_t> predefine_sequential_paths(const vector<vector<Vec2d<>>> &paths)
    {
        vector<size_t> indexes;

        // Überprüfen, ob paths leer ist
        if (paths.empty())
        {
            return indexes; // Leerer Vektor zurückgeben
        }

        indexes.push_back(predefine_path(paths.front()));

        for (size_t i = 1; i < paths.size(); ++i)
        {
            vector<Vec2d<>> control_points = paths[i];
            control_points.insert(control_points.begin(), paths[i - 1].back());
            indexes.push_back(predefine_path(control_points));
        }
        return indexes;
    }

    // ----------------- Common functions needed when starting the driving action -----------------

    bool is_last_target_position_close_enough_to_current_position()
    {
        // If we are close enough to the last target position we can assume that we begin
        // this driving from the last target position
        const Position current_position = pinpoint::get_position();

        const Position<> distance_to_last_target_position =
            current_position - drive_to_position_controller::target_position;
        if (distance_to_last_target_position.total_distance() < max_deviations.coordinates &&
            abs(distance_to_last_target_position.rotation) < max_deviations.rotation)
        {
            starting_position = drive_to_position_controller::target_position;
            return true;
        }
        else
        {
            starting_position = current_position;
            return false;
        }
    }

    void make_control_points(const Vec2d<> &starting_position,
                             vector<Vec2d<>> &control_points)
    {
        // Calculates all control points and sets them
        control_points.insert(control_points.begin(), starting_position);
    }

    void make_control_points(const Vec2d<> &starting_position,
                             vector<Vec2d<>> &control_points,
                             const Vec2d<> &target_position)
    {
        // Calculates all control points and sets them
        control_points.insert(control_points.begin(), starting_position);
        control_points.push_back(target_position);
    }

    size_t index_of_predefined_path(const vector<Vec2d<>> &control_points)
    {
        for (size_t i = 0; i < control_points_predefined_curves.size(); i++)
        {
            if (control_points_predefined_curves[i] == control_points)
                return i;
        }
        return -1;
    }

    // ----------------- The two different types to handle curves -----------------

    void handle_predefined_curves(const size_t &index_of_predefined_curve,
                                  const Degree &target_rotation,
                                  const chrono::steady_clock::duration &timeout, const bool &rotation_should_follow_path,
                                  const bool &arrive_forwards = true)
    {
        const vector<Vec2d<>> &points_on_curve = points_on_predefined_curves[index_of_predefined_curve];

        target_position = {points_on_curve.back(), target_rotation};

        // Adjusting the percent per loop to how fast we'll drive and how far we'll drive
        // What this changes is explained at the percent_per_loop_multiplier declaration
        const double percent_per_loop = 1 / static_cast<double>(points_on_curve.size());

        // This stops the old thread as soon as possible if there is one
        if (!position_reached)
            wait_until_position_is_reached(-1ms);

        // Sets position_reached to false
        position_reached = false;

        // Sets the new timeout and notes the start time
        drive_to_position_controller::timeout = timeout;
        start_time = chrono::steady_clock::now();

        // If we just need to turn the normal drive to position won't work, that's why there is a custom turn
        // to rotation for such a case
        if ((starting_position - target_position).total_distance() > max_deviations.coordinates)
            drive_along_path(index_of_predefined_curve, percent_per_loop,
                             rotation_should_follow_path, arrive_forwards);
        else
            turn_to_rotation();

        // If the timeout is zero that means we drive asynchronously
        if (timeout != 0ms)
            wait_until_position_is_reached(timeout);
    }

    void handle_non_predefined_curves(vector<Vec2d<>> &control_points,
                                      const Position<> &target_position,
                                      const chrono::steady_clock::duration &timeout,
                                      const bool &rotation_should_follow_path,
                                      const bool &arrive_forwards = true)
    {
        drive_to_position_controller::target_position = target_position;

        // Adjusting the percent per loop to how fast we'll drive and how far we'll drive
        // What this changes is explained at the percent_per_loop_multiplier declaration
        const double percent_per_loop = percent_per_loop_multiplier *
                                        (max_motor_power / estimate_curve_length(control_points));

        // This stops the old thread as soon as possible if there is one
        if (!position_reached)
            wait_until_position_is_reached(-1ms);

        // Sets position_reached to false
        position_reached = false;

        // Sets the new timeout and notes the start time
        drive_to_position_controller::timeout = timeout;
        start_time = chrono::steady_clock::now();

        // If we just need to turn the normal drive to position won't work, that's why there is a custom turn
        // to rotation for such a case
        if ((starting_position - target_position).total_distance() > max_deviations.coordinates)
            drive_to_position(control_points, percent_per_loop,
                              rotation_should_follow_path,
                              arrive_forwards);
        else
            turn_to_rotation();

        // If the timeout is zero that means we drive asynchronously
        if (timeout != 0ms)
            wait_until_position_is_reached(timeout);
    }

    // ----------------- Initiating a driving action -----------------

    void drive_to_position(Position<> target_position,
                           vector<Vec2d<>> control_points,
                           const chrono::steady_clock::duration &timeout)
    {
        reverse_if_blue_alliance(target_position, control_points);

        const bool last_target_position_close_enough_to_current_position = is_last_target_position_close_enough_to_current_position();

        make_control_points({starting_position.x, starting_position.y}, control_points,
                            {target_position.x, target_position.y});

        if (last_target_position_close_enough_to_current_position)
        {
            size_t index_of_path = index_of_predefined_path(control_points);
            if (index_of_path != -1)
            {
                handle_predefined_curves(index_of_path, target_position.rotation, timeout, false);
                return;
            }
        }

        handle_non_predefined_curves(control_points, target_position, timeout, false);
    }

    void drive_to_position(const Position<> &target_position, const chrono::steady_clock::duration &timeout)
    {
        drive_to_position(target_position, {}, timeout);
    }

    void drive_to_position(vector<Vec2d<>> control_points, const bool &arrive_forwards,
                           const chrono::steady_clock::duration &timeout)
    {
        reverse_if_blue_alliance(control_points);

        const bool last_target_position_close_enough_to_current_position = is_last_target_position_close_enough_to_current_position();
        make_control_points({starting_position.x, starting_position.y}, control_points);

        const Vec2d<> &last_control_point = control_points.back();
        const Degree rotation = calc_abs_target_rotation(pinpoint::get_position().rotation,
                                                         calc_tangent_angle(make_tuple(
                                                             control_points[control_points.size() - 2],
                                                             last_control_point)),
                                                         arrive_forwards);

        if (last_target_position_close_enough_to_current_position)
        {
            size_t index_of_path = index_of_predefined_path(control_points);
            if (index_of_path != -1)
            {
                handle_predefined_curves(index_of_path, rotation, timeout, true, arrive_forwards);
                return;
            }
        }

        handle_non_predefined_curves(control_points, {last_control_point, rotation}, timeout, true,
                                     arrive_forwards);
    }

    void drive_to_position(const size_t &index_of_path, Degree target_rotation, const chrono::steady_clock::duration &timeout)
    {
        reverse_if_blue_alliance(target_rotation);

        starting_position = {control_points_predefined_curves[index_of_path].front(), pinpoint::get_position().rotation};
        handle_predefined_curves(index_of_path, target_rotation, timeout, false);
    }

    void drive_to_position(const size_t &index_of_path, const bool &arrive_forwards,
                           const chrono::steady_clock::duration &timeout)
    {
        const vector<Vec2d<>> &control_points = control_points_predefined_curves[index_of_path];
        const Vec2d<> &last_control_point = control_points.back();
        starting_position = {control_points_predefined_curves[index_of_path].front(), calc_abs_target_rotation(
                                                                                          pinpoint::get_position().rotation,
                                                                                          calc_tangent_angle(
                                                                                              speed_points_on_predefined_curves[index_of_path].front()),
                                                                                          arrive_forwards)};
        Degree rotation = calc_abs_target_rotation(pinpoint::get_position().rotation,
                                                   calc_tangent_angle(make_tuple(
                                                       control_points[control_points.size() -
                                                                      2],
                                                       last_control_point)),
                                                   arrive_forwards);

        handle_predefined_curves(index_of_path, rotation, timeout, true, arrive_forwards);
    }

    void hold_position(Position<> position, const chrono::steady_clock::duration &time)
    {
        reverse_if_blue_alliance(position);

        target_position = position;

        // This stops the old thread as soon as possible if there is one
        if (!position_reached)
            wait_until_position_is_reached(-1ms);

        // Sets position_reached to false
        position_reached = false;

        // Sets the new timeout and notes the start time
        timeout = time;
        start_time = chrono::steady_clock::now();

        hold_position();

        // If the timeout is zero that means we drive asynchronously
        if (time != 0ms)
            wait_until_position_is_reached(time);
    }

    // ----------------- Methods to control an async driving action -----------------

    void set_timeout(const chrono::steady_clock::duration &timeout)
    {
        drive_to_position_controller::timeout = timeout;
    }

    void wait_until_position_is_reached(const chrono::steady_clock::duration &timeout)
    {
        if (position_reached)
            return;

        drive_to_position_controller::timeout = timeout;

        threads_waiting_until_position_reached++;
        sem_wait(&wait_until_position_reached);
        threads_waiting_until_position_reached--;

        if (threads_waiting_until_position_reached > 0)
            sem_post(&wait_until_position_reached);
    }
}