//
// Created by fnlg on 20.05.2024.
//

#ifndef FTCROBOTCONTROLLER_QUATERNION_H
#define FTCROBOTCONTROLLER_QUATERNION_H

namespace sdk {

    struct Quaternion {
        // TODO: This is not complete
        float w = 0.0F;
        float x = 0.0F;
        float y = 0.0F;
        float z = 0.0F;
        long acquisition_time = 0L;

        Quaternion() = default;

        Quaternion(const float &w, const float &x, const float &y, const float &z,
                   const long &acquisition_time) : w(w), x(x), y(y), z(z),
                                                   acquisition_time(acquisition_time) {}
    };

} // sdk

#endif //FTCROBOTCONTROLLER_QUATERNION_H
