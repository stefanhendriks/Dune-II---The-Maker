#ifndef RECTANGLE
#define RECTANGLE

#include "point.h"

// NOTE: This class assumes start is up left opposed to end (down-right)
class Rectangle {
  public:
    Rectangle() { start = Point(0,0); end = Point(0,0); }
    Rectangle(Point start, Point end) { this->start = start; this->end = end; }
    Rectangle(const Rectangle *other) { this->start = other->start; this->end = other->end; }
    Rectangle(const Rectangle &other) { this->start = other.start; this->end = other.end; }

    bool is_point_within(const Point& p) const {
      return ((p.x >= start.x && p.x <= end.x) && (p.y >= start.y && p.y <= end.y));
    }

    Point start;
    Point end;
};

#endif
