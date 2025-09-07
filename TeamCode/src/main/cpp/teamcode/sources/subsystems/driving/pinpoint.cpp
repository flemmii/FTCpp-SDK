#include "subsystems/driving/pinpoint.hpp"

using namespace std;
using namespace sdk;

namespace pinpoint
{
    GoBilda_pinpoint_driver pinpoint;
    BNO055IMU imu;

    void init()
    {
        imu = hardware_map::get<BNO055IMU>("imu");
        imu.initialize(BNO055IMU::Parameters());
        /*
        pinpoint = hardware_map::get<GoBilda_pinpoint_driver>("pinpoint");
        pinpoint.reset_pos_and_IMU();
        pinpoint.set_offsets(-Millimetre(DEAD_WHEEL_FORWARDS_OFFSET_X).Value(), Millimetre(DEAD_WHEEL_SIDEWARDS_OFFSET_Y).Value());
        pinpoint.set_encoder_directions(DEAD_WHEEL_FORWARDS, DEAD_WHEEL_SIDEWARDS);
        pinpoint.set_encoder_resolution(PODS);
        */
    }

    Position<true, Speed, AngularSpeed> last_speed;
    chrono::steady_clock::duration delta_t = MIN_QUERY_RATE;
    chrono::steady_clock::time_point last_query_time = chrono::steady_clock::now() - MIN_QUERY_RATE - 1ms;

    Position<true, Speed, AngularSpeed> get_speed_without_updating()
    {
        static Position<true, Speed, AngularSpeed> speed_cache(0_m / 1_s, 0_m / 1_s, 0_deg / 1_s);
        static chrono::steady_clock::time_point last_update_time = chrono::steady_clock::now();

        if (last_update_time < last_query_time)
        {
            last_update_time = chrono::steady_clock::now();
            const Pose2D current_speed = pinpoint.get_velocity();
            speed_cache = {Speed(-current_speed.getY(Distance_unit::METER)),
                           Speed(current_speed.getX(Distance_unit::METER)),
                           AngularSpeed(-current_speed.getHeading(Angle_unit::DEGREES))};
        }
        return speed_cache;
    }

    void update_if_allowed()
    {
        const chrono::steady_clock::time_point current_time = chrono::steady_clock::now();

        if (current_time - last_query_time > MIN_QUERY_RATE)
        {
            last_speed = get_speed_without_updating();
            delta_t = current_time - last_query_time;
            // pinpoint.update();
            last_query_time = current_time;
        }
    }

    void set_position(const Millimetre &x_position, const Millimetre &y_position, Degree rotation)
    {
        // pinpoint.set_position({Distance_unit::MM, y_position.Value(), -x_position.Value(), Angle_unit::DEGREES, -rotation.Value()});
    }

    void set_position(const Position<> &position)
    {
        set_position(position.x, position.y, position.rotation);
    }

    void set_coordinates(const Millimetre &x_position, const Millimetre &y_position)
    {
        set_position(x_position, y_position, get_position().rotation);
    }

    void set_rotation(Degree rotation)
    {
        const Position<> current_position = get_position();
        set_position(current_position.x, current_position.y, rotation);
    }

    Position<> get_position()
    {
        return {0_mm, 0_mm, Radian(-imu.get_angular_orientation().first_angle)};
        static Position<> position_cache(0_mm, 0_mm, 0_deg);
        static chrono::steady_clock::time_point last_update_time = chrono::steady_clock::now();

        update_if_allowed();
        if (last_update_time < last_query_time)
        {
            last_update_time = chrono::steady_clock::now();
            const Pose2D current_pose = pinpoint.get_position();
            position_cache = {Millimetre(-current_pose.getY(Distance_unit::MM)),
                              Millimetre(current_pose.getX(Distance_unit::MM)),
                              Degree(-current_pose.getHeading(Angle_unit::DEGREES))};
        }
        return position_cache;
    }

    Position<true, Speed, AngularSpeed> get_speed()
    {
        return {0_mm / 1_s, 0_mm / 1_s, 0_deg / 1_s};
        update_if_allowed();
        return get_speed_without_updating();
    }

    Position<true, ::Acceleration, AngularAcceleration> get_acceleration()
    {
        return (get_speed() - last_speed) / Millisecond(chrono::duration_cast<chrono::milliseconds>(delta_t).count());
    }
}