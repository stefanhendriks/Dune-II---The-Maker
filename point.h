#ifndef POINT
#define POINT

class Point {
  public:
    Point(int x, int y);

    int x() { return _x; }

  private:
    int _x, _y;
};

#endif
