#ifndef VECTOR2D
#define VECTOR2D

#include "fpoint.h"

// vectors represent direction, not a position
class Vector2D {

  public:
    Vector2D();
    Vector2D(float x, float y);
    Vector2D(const FPoint &from, const FPoint &to);

    float length();
    Vector2D normalize();

    float x, y;

};

#endif
