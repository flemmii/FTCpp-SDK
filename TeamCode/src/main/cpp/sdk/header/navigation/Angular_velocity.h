//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_ANGULAR_VELOCITY_H
#define FTCROBOTCONTROLLER_ANGULAR_VELOCITY_H

namespace sdk {

    struct Angular_velocity {
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

#endif //FTCROBOTCONTROLLER_ANGULAR_VELOCITY_H
