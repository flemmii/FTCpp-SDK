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

        Quaternion(float w, float x, float y, float z, long acquisition_time);
    };

} // sdk

#endif //FTCROBOTCONTROLLER_QUATERNION_H
