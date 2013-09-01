#include <iostream>

#include "eventfactory.h"

using namespace std;

EventFactory::EventFactory() {
  last_sent_camera_movement.vec_x = 0;
  last_sent_camera_movement.vec_y = 0;
}

void EventFactory::pushMoveUnitEvent(int screen_x, int screen_y) {
  D2TMMoveUnitStruct *s = new D2TMMoveUnitStruct;
  s->screen_x = screen_x;
  s->screen_y = screen_y;
  push_event(D2TM_MOVE_UNIT, s);
}

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
  s->screen_position = Point(x,y);
  push_event(D2TM_SELECT, s);
}

void EventFactory::pushMoveCameraEvent(int vec_x, int vec_y) {
  if (vec_x == last_sent_camera_movement.vec_x && vec_y == last_sent_camera_movement.vec_y) {
    return;
  }

  D2TMMoveCameraStruct *s = new D2TMMoveCameraStruct;
  s->vec_x = vec_x;
  s->vec_y = vec_y;
  push_event(D2TM_MOVE_CAMERA, s);

  last_sent_camera_movement.vec_x = vec_x;
  last_sent_camera_movement.vec_y = vec_y;
}
