#include <iostream>

#include "eventfactory.h"

using namespace std;

void EventFactory::pushBoxSelectEvent(int startX, int startY, int endX, int endY) {

  if (endX < startX) swap(endX, startX);
  if (endY < startY) swap(endY, startY);

  D2TMBoxSelectStruct *s = new D2TMBoxSelectStruct;
  s->start_x = startX;
  s->start_y = startY;
  s->end_x = endX;
  s->end_y = endY;

  push_event(D2TM_BOX_SELECT, s);
}

void EventFactory::pushSelectEvent(int x, int y) {
  D2TMSelectStruct *s = new D2TMSelectStruct;
  s->x = x;
  s->y = y;

  push_event(D2TM_SELECT, s);
}

void EventFactory::pushMoveCameraEvent(float vec_x, float vec_y) {
  D2TMMoveCameraStruct *s = new D2TMMoveCameraStruct;
  s->vec_x = vec_x;
  s->vec_y = vec_y;
  push_event(D2TM_MOVE_CAMERA, s);
}
