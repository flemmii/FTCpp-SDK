//
// Created by fnlg on 20.05.2024.
//

#include "navigation/Angular_velocity.h"

namespace sdk {
    Angular_velocity::Angular_velocity(float x_rotation_rate, float y_rotation_rate,
                                       float z_rotation_rate) : x_rotation_rate(x_rotation_rate),
                                                                y_rotation_rate(y_rotation_rate),
                                                                z_rotation_rate(z_rotation_rate) {}
} // sdk