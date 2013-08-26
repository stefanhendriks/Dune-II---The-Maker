#ifndef EVENT_FACTORY
#define EVENT_FACTORY

#include "SDL/SDL.h"

#define D2TM_SELECT          1024
#define D2TM_DESELECT        1025
#define D2TM_BOX_SELECT      1026

typedef struct {
  int start_x;
  int start_y;
  int end_x;
  int end_y;
} D2TMBoxSelectStruct;

typedef struct {
  int x;
  int y;
} D2TMSelectStruct;


class EventFactory {

  public:
    void pushBoxSelectEvent(int startX, int startY, int endX, int endY);
    void pushSelectEvent(int x, int y);
    void pushDeselectEvent() { push_event(D2TM_DESELECT); }

  private:

    bool push_event(int code, void* data1, void* data2) {
      SDL_Event clickEvent;

      clickEvent.type = SDL_USEREVENT;
      clickEvent.user.code = code;
      clickEvent.user.data1 = data1;
      clickEvent.user.data2 = data2;

      SDL_PushEvent(&clickEvent);

      return true;
    }

    bool push_event(int code, void* data1) {
      return push_event(code, data1, NULL);
    }

    bool push_event(int code) {
      return push_event(code, NULL, NULL);
    }

};



#endif
