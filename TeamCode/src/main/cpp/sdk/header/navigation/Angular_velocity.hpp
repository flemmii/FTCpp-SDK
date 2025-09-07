#pragma once

namespace sdk
{

    struct Angular_velocity
    {
        // TODO: This is not complete
        float x_rotation_rate;
        float y_rotation_rate;
        float z_rotation_rate;

        Angular_velocity(const float &x_rotation_rate, const float &y_rotation_rate,
                         const float &z_rotation_rate) : x_rotation_rate(x_rotation_rate),
                                                         y_rotation_rate(y_rotation_rate),
                                                         z_rotation_rate(z_rotation_rate) {}
    };

} // sdk
