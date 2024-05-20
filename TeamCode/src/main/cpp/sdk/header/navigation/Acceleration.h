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

        Acceleration(double x_accel, double y_accel, double z_accel);
    };

} // sdk

#endif //FTCROBOTCONTROLLER_ACCELERATION_H
