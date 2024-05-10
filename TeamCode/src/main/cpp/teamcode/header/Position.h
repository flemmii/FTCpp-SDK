#ifndef FTCROBOTCONTROLLER_POSITION_H
#define FTCROBOTCONTROLLER_POSITION_H


#include <string>

#include "Vec2d.h"

using namespace std;

struct Position {
    Position() = default;

    double x;
    double y;
    double rotation;

    Position(double x, double y, double rotation) : x(x), y(y), rotation(rotation) {}

    Position(Vec2d vec, double rotation) : x(vec.x), y(vec.y), rotation(rotation) {}

    void set(double x, double y, double rotation);

    Position set_x(double x);

    Position set_y(double y);

    Position set_rotation(double rotation);

    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] double total_distance() const;

    friend Position operator-(const Position &lhs, const Position &rhs);

    friend Position operator-(const Position &lhs, const double &value);

    friend Position operator+(const Position &lhs, const Position &rhs);

    friend Position operator+(const Position &lhs, const Vec2d &rhs);

    friend Position operator+(const Position &lhs, const double &value);

    Position operator+=(const Position &rhs);

    Position operator+=(const Vec2d &rhs);

    friend Position operator-(const Position &lhs, const Vec2d &rhs);

    friend Position operator*(const Position &lhs, const Position &rhs);

    friend Position operator*(const Position &lhs, const double &value);

    friend Position operator/(const Position &lhs, const Position &rhs);

    friend Position fabs(const Position &lhs);

    friend Position sgn(const Position &lhs);

    // Comparison operators need to be in the header
    friend bool operator<(const Position &lhs, const Position &rhs) {
        return lhs.x < rhs.x &&
               lhs.y < rhs.y &&
               lhs.rotation < rhs.rotation;
    }
};


#endif //FTCROBOTCONTROLLER_POSITION_H
