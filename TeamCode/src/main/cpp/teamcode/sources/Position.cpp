#include "../header/Position.h"

void Position::set(double x, double y, double rotation) {
    this->x = x;
    this->y = y;
    this->rotation = rotation;
}

Position Position::set_x(double x) {
    this->x = x;
    return *this;
}

Position Position::set_y(double y) {
    this->y = y;
    return *this;
}

Position Position::set_rotation(double rotation) {
    this->rotation = rotation;
    return *this;
}

std::string Position::to_string() const {
    return (::to_string(x) + "/" + ::to_string(y) + "/" + ::to_string(rotation));
}

double Position::total_distance() const {
    return sqrt(x * x + y * y);
}

Position operator-(const Position &lhs, const Position &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.rotation - rhs.rotation};
}

Position operator-(const Position &lhs, const double &value) {
    return {lhs.x - value, lhs.y - value, lhs.rotation - value};
}

Position operator+(const Position &lhs, const Position &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.rotation + rhs.rotation};
}

Position operator+(const Position &lhs, const double &value) {
    return {lhs.x + value, lhs.y + value, lhs.rotation + value};
}

Position Position::operator+=(const Position &rhs) {
    x += rhs.x;
    y += rhs.y;
    rotation += rhs.rotation;
    return *this;
}

Position Position::operator+=(const Vec2d &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Position operator-(const Position &lhs, const Vec2d &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.rotation};
}

Position operator+(const Position &lhs, const Vec2d &rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.rotation};
}

Position operator*(const Position &lhs, const Position &rhs) {
    return {lhs.x * rhs.x, lhs.y * rhs.y, lhs.rotation * rhs.rotation};
}

Position operator*(const Position &lhs, const double &value) {
    return {lhs.x * value, lhs.y * value, lhs.rotation * value};
}

Position operator/(const Position &lhs, const Position &rhs) {
    return {lhs.x / rhs.x, lhs.y / rhs.y, lhs.rotation / rhs.rotation};
}

Position fabs(const Position &lhs) {
    return {fabs(lhs.x), fabs(lhs.y), fabs(lhs.rotation)};
}

Position sgn(const Position &lhs) {
    return {static_cast<double>((lhs.x > 0) - (lhs.x < 0)),
            static_cast<double>((lhs.y > 0) - (lhs.y < 0)),
            static_cast<double>((lhs.rotation > 0) - (lhs.rotation < 0))};
}
