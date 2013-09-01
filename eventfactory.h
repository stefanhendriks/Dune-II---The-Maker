#ifndef EVENT_FACTORY
#define EVENT_FACTORY

#include "SDL/SDL.h"
#include "point.h"

#define D2TM_SELECT          1024
#define D2TM_DESELECT        1025
#define D2TM_BOX_SELECT      1026
#define D2TM_MOVE_CAMERA     1027
#define D2TM_MOVE_UNIT       1028

typedef struct {
  Point start;
  Point end;
} D2TMBoxSelectStruct;

typedef struct {
  Point screen_position;
} D2TMSelectStruct;

// camera directions
const int D2TM_CAMERA_MOVE_UP = 1;
const int D2TM_CAMERA_MOVE_DOWN = 2;
const int D2TM_CAMERA_MOVE_LEFT = 3;
const int D2TM_CAMERA_MOVE_RIGHT = 4;
const int D2TM_CAMERA_STOP_MOVING = 5;

typedef struct {
  Point vector;
} D2TMMoveCameraStruct;

typedef struct {
  Point screen_position;
} D2TMMoveUnitStruct;

class EventFactory {

  public:
    EventFactory();

    void pushBoxSelectEvent(int startX, int startY, int endX, int endY);
    void pushSelectEvent(int x, int y);
    void pushDeselectEvent() { push_event(D2TM_DESELECT); }
    void pushMoveCameraEvent(int vec_x, int vec_y);
    void pushMoveUnitEvent(int screen_x, int screen_y);
    void pushQuitEvent() { push_sdl_event(SDL_QUIT, 0, NULL, NULL); }

  private:

    bool push_sdl_event(int type, int code, void* data1, void *data2) {
      SDL_Event clickEvent;

      clickEvent.type = type;
      clickEvent.user.code = code;
      clickEvent.user.data1 = data1;
      clickEvent.user.data2 = data2;

      SDL_PushEvent(&clickEvent);

      return true;
    }

    bool push_event(int code, void* data1, void* data2) {
      return push_sdl_event(SDL_USEREVENT, code, data1, data2);
    }

    bool push_event(int code, void* data1) {
      return push_event(code, data1, NULL);
    }

    bool push_event(int code) {
      return push_event(code, NULL, NULL);
    }

    Point last_sent_camera_vector;
};



#endif
