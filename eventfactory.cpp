#include <iostream>

#include "eventfactory.h"

using namespace std;

EventFactory::EventFactory() {
}

void EventFactory::pushMoveUnitEvent(int screen_x, int screen_y) {
  D2TMMoveUnitStruct *s = new D2TMMoveUnitStruct;
  s->screen_position = Point(screen_x, screen_y);
  push_event(D2TM_MOVE_UNIT, s);
}

void EventFactory::pushBoxSelectEvent(int startX, int startY, int endX, int endY) {
  if (endX < startX) swap(endX, startX);
  if (endY < startY) swap(endY, startY);

  D2TMBoxSelectStruct *s = new D2TMBoxSelectStruct;
  s->start = Point(startX, startY);
  s->end = Point(endX, endY);

  push_event(D2TM_BOX_SELECT, s);
}

void EventFactory::pushSelectEvent(int x, int y) {
  D2TMSelectStruct *s = new D2TMSelectStruct;
  s->screen_position = Point(x,y);
  push_event(D2TM_SELECT, s);
}

void EventFactory::pushMoveCameraEvent(int vec_x, int vec_y) {
  Point move_vector = Point(vec_x, vec_y);
  if (move_vector == last_sent_camera_vector) {
    return;
  }

  D2TMMoveCameraStruct *s = new D2TMMoveCameraStruct;
  s->vector = move_vector;
  push_event(D2TM_MOVE_CAMERA, s);

  last_sent_camera_vector = move_vector;
}
