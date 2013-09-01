#ifndef POINT
#define POINT

class Point {
  public:
    Point() { x=0; y=0; }
    Point(int x, int y) { this->x = x; this->y = y; }
    Point(const Point* other) { this->x = other->x; this->y = other->y; }
    Point(const Point &other) { this->x = other.x; this->y = other.y; }

    int x, y;
};

inline bool operator==(const Point& lhs, const Point& rhs){
  return (lhs.x == rhs.x && lhs.y == rhs.y);
}
inline bool operator!=(const Point& lhs, const Point& rhs){return !operator==(lhs,rhs);}

inline bool operator< (const Point& lhs, const Point& rhs){
  return (lhs.x < rhs.x || lhs.y < rhs.y);
}

inline bool operator> (const Point& lhs, const Point& rhs){return  operator< (rhs,lhs);}
inline bool operator<=(const Point& lhs, const Point& rhs){return !operator> (lhs,rhs);}
inline bool operator>=(const Point& lhs, const Point& rhs){return !operator< (lhs,rhs);}

#endif
