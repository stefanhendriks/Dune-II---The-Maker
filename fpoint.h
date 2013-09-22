#ifndef FPOINT
#define FPOINT

#include "point.h"

const float FLOAT_COMPARISON_THRESHOLD = 0.001F;

class FPoint {
  public:
    FPoint() { x=0.0F; y=0.0F; }
    FPoint(float x, float y) { this->x = x; this->y = y; }
    FPoint(const FPoint* other) { this->x = other->x; this->y = other->y; }
    FPoint(const FPoint &other) { this->x = other.x; this->y = other.y; }
    FPoint(const Point* other) { this->x = other->x; this->y = other->y; }
    FPoint(const Point &other) { this->x = other.x; this->y = other.y; }

    float x, y;
};

inline bool operator==(const FPoint& lhs, const FPoint& rhs){
  float diffX = fabs(lhs.x - rhs.x);
  float diffY = fabs(lhs.y - rhs.y);
  return (diffX < FLOAT_COMPARISON_THRESHOLD && diffY < FLOAT_COMPARISON_THRESHOLD);
}
inline bool operator!=(const FPoint& lhs, const FPoint& rhs){return !operator==(lhs,rhs);}

inline bool operator< (const FPoint& lhs, const FPoint& rhs){
  return (lhs.x < rhs.x || lhs.y < rhs.y);
}

inline bool operator> (const FPoint& lhs, const FPoint& rhs){return  operator< (rhs,lhs);}
inline bool operator<=(const FPoint& lhs, const FPoint& rhs){return !operator> (lhs,rhs);}
inline bool operator>=(const FPoint& lhs, const FPoint& rhs){return !operator< (lhs,rhs);}

inline FPoint operator+(const FPoint& lhs, const FPoint& rhs) {
  FPoint result = lhs;
  result.x += rhs.x;
  result.y += rhs.y;
  return result;
}

inline FPoint operator-(const FPoint& lhs, const FPoint& rhs) {
  FPoint result = lhs;
  result.x -= rhs.x;
  result.y -= rhs.y;
  return result;
}
#endif
