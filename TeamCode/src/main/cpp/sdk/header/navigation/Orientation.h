//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_ORIENTATION_H
#define FTCROBOTCONTROLLER_ORIENTATION_H

namespace sdk {

    struct Orientation {
        //TODO: This is not complete
        float first_angle;
        float second_angle;
        float third_angle;

        Orientation(float first_angle, float second_angle, float third_angle);
    };

} // sdk

#endif //FTCROBOTCONTROLLER_ORIENTATION_H
