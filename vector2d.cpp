#include "vector2d.h"
#include <math.h>

Vector2D::Vector2D(const FPoint &from, const FPoint &to) {
  this->x = to.x - from.x;
  this->y = to.y - from.y;
}

Vector2D::Vector2D(float x, float y) {
  this->x = x;
  this->y = y;
}

float Vector2D::length() {
  return sqrt((x*x)+(y*y));
}

Vector2D Vector2D::normalize() {
  float length = this->length();

  if(length != 0) {
    return Vector2D(x / length, y / length);
  }

  return Vector2D(0.0F, 0.0F);
}
