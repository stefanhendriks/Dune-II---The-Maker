#ifndef GAME_H
#define GAME_H

class Game {

  public:
     Game();

     int execute();

   private:
      int init();
      void onEvent(SDL_Event* event);
      void updateState();
      void render();
      int cleanup();

      bool playing;
      SDL_Surface *screen;
};

#endif
