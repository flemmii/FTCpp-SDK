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

        Orientation(const float &first_angle, const float &second_angle, const float &third_angle)
                : first_angle(first_angle), second_angle(second_angle), third_angle(third_angle) {}
    };

} // sdk

#endif //FTCROBOTCONTROLLER_ORIENTATION_H
