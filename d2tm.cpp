#include <ctime>
#include "SDL/SDL.h"
#include "game.h"

int main( int argc, char* args[] ) {
  unsigned int t = (unsigned int) time(0);
  srand(t);
  Game game;
  return game.execute();
}
