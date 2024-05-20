//
// Created by fnlg on 20.05.2024.
//

#include "navigation/Quaternion.h"

namespace sdk {
    Quaternion::Quaternion(float w, float x, float y, float z, long acquisition_time) : w(w), x(x),
                                                                                        y(y), z(z),
                                                                                        acquisition_time(
                                                                                                acquisition_time) {}
} // sdk