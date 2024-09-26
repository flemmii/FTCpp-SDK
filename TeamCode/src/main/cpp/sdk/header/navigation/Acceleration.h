//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_ACCELERATION_H
#define FTCROBOTCONTROLLER_ACCELERATION_H

namespace sdk {

    struct Acceleration {
        //TODO: This is not complete
        double x_accel;
        double y_accel;
        double z_accel;

        Acceleration(const double &x_accel, const double &y_accel, const double &z_accel) : x_accel(
                x_accel),
                                                                                            y_accel(y_accel),
                                                                                            z_accel(z_accel) {}
    };

} // sdk

#endif //FTCROBOTCONTROLLER_ACCELERATION_H
