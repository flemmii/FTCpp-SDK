//
// Created by Frosch PC - V2 on 12.03.2024.
//

#include "../header/Vec2d.h"

using namespace std;

void Vec2d::set(double x, double y) {
    this->x = x;
    this->y = y;
}

string Vec2d::to_string() const {
    return ::to_string(x) + "/" + ::to_string(y);
}

Vec2d Vec2d::remap(Vec2d_remap_order remap_order) {
    if (remap_order == Vec2d_remap_order::XY_XY) {
        return *this;
    } else if (remap_order == Vec2d_remap_order::YX_XY) {
        return {y, x};
    }
    return *this;
}

double Vec2d::total_distance() const {
    return sqrt(x * x + y * y);
}

Vec2d operator+(const Vec2d &lhs, const Vec2d &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

Vec2d operator-(const Vec2d &lhs, const double &value) {
    return {lhs.x - value, lhs.y - value};
}

Vec2d operator-(const Vec2d &lhs, const Vec2d &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Vec2d operator*(const Vec2d &lhs, const double &value) {
    return {lhs.x * value, lhs.y * value};
}

Vec2d operator*(const Vec2d &lhs, const Vec2d &rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y};
}

Vec2d operator/(const Vec2d &lhs, const Vec2d &rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y};
}

Vec2d fabs(const Vec2d &lhs) {
    return {fabs(lhs.x), fabs(lhs.y)};
}

Vec2d sgn(const Vec2d &lhs) {
    return {static_cast<double>((lhs.x > 0) - (lhs.x < 0)),
            static_cast<double>((lhs.y > 0) - (lhs.y < 0))};
}
