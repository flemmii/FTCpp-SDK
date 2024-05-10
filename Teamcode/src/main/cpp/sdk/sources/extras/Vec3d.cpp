//
// Created by fnlg on 27.01.2024.
//

#include "extras/Vec3d.h"

using namespace std;

void Vec3d::set(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

string Vec3d::to_string() const {
    return ::to_string(x) + "/" + ::to_string(y) + "/" + ::to_string(z);
}

Vec3d Vec3d::remap(Vec3d_remap_order remap_order) {
    if (remap_order == Vec3d_remap_order::XYZ_XYZ) {
        return *this;
    } else if (remap_order == Vec3d_remap_order::XZY_XYZ) {
        return {x, z, y};
    } else if (remap_order == Vec3d_remap_order::YXZ_XYZ) {
        return {y, x, z};
    } else if (remap_order == Vec3d_remap_order::YZX_XYZ) {
        return {y, z, x};
    } else if (remap_order == Vec3d_remap_order::ZYX_XYZ) {
        return {z, y, x};
    } else if (remap_order == Vec3d_remap_order::ZXY_XYZ) {
        return {z, x, y};
    }
    return *this;
}