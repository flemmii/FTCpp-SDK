//
// Created by Frosch PC - V2 on 12.03.2024.
//

#ifndef FTCROBOTCONTROLLER_VEC2D_H
#define FTCROBOTCONTROLLER_VEC2D_H

#include <string>

using namespace std;

struct Vec2d {
    double x, y;

    Vec2d(double x, double y) : x(x), y(y) {}

    enum class Vec2d_remap_order {
        XY_XY,
        YX_XY
    };

    void set(double x, double y);

    [[nodiscard]] std::string to_string() const;

    Vec2d remap(Vec2d_remap_order remap_order);

    [[nodiscard]] double total_distance() const;

    friend Vec2d operator+(const Vec2d &lhs, const Vec2d &rhs);

    friend Vec2d operator-(const Vec2d &lhs, const double &value);

    friend Vec2d operator-(const Vec2d &lhs, const Vec2d &rhs);

    friend Vec2d operator*(const Vec2d &lhs, const double &value);

    friend Vec2d operator*(const Vec2d &lhs, const Vec2d &rhs);

    friend Vec2d operator/(const Vec2d &lhs, const Vec2d &rhs);

    friend Vec2d sgn(const Vec2d &lhs);

    friend Vec2d fabs(const Vec2d &lhs);

    // Comparison operators need to be in the header
    friend bool operator<(const Vec2d &lhs, const Vec2d &rhs) {
        return lhs.x < rhs.x &&
               lhs.y < rhs.y;
    }
};


#endif //FTCROBOTCONTROLLER_VEC2D_H
