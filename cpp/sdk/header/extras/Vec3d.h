//
// Created by fnlg on 27.01.2024.
//

#ifndef FTCROBOTCONTROLLER_VEC3D_H
#define FTCROBOTCONTROLLER_VEC3D_H


#include <string>

using namespace std;

struct Vec3d {
    double x, y, z;

    Vec3d(double x, double y, double z) : x(x), y(y), z(z) {}

    enum class Vec3d_remap_order {
        XYZ_XYZ,
        XZY_XYZ,
        YXZ_XYZ,
        YZX_XYZ,
        ZYX_XYZ,
        ZXY_XYZ
    };

    void set(double x, double y, double z);

    [[nodiscard]] std::string to_string() const;

    Vec3d remap(Vec3d_remap_order remap_order);
};


#endif //FTCROBOTCONTROLLER_VEC3D_H
