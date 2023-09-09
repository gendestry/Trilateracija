#pragma once
#include "Util.h"

struct Point {
    float x, y;

    inline const char* toString() { return concat("(x:", x, ", y:", y, ")"); }
};

Point trilaterate(const Point& a, const Point& b, const Point& c, float ra, float rb, float rc) {
    // check for invalid position
    if(b.x == a.x) {
        // std::cerr << "Oddajnika A in B ne smeta imeti enakih x koordniat" << std::endl;
        return Point();
    }

    // constants
    float c1 = 2.0f * (b.x - a.x);
    float c2 = 2.0f * (b.y - a.y);
    float c3 = a.x * a.x + a.y * a.y - ra * ra - b.x * b.x - b.y * b.y + rb * rb;
    float c4 = 2.0f * (c.x - b.x);
    float c5 = 2.0f * (c.y - b.y);
    float c6 = b.x * b.x + b.y * b.y - rb * rb - c.x * c.x - c.y * c.y + rc * rc;

    // get y, x
    Point p;
    p.y = (c3 * c4 - c1 * c6) / (c1 * c5 - c2 * c4);
    p.x = (-p.y * c2 - c3) / c1;

    return p;
}
