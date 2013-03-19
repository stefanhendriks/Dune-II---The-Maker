#include "SDL.h"
 
int main( int argc, char* args[] )
{
    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );
 
    //Create an SDL_Surface and display it
    SDL_Surface* screen = NULL;
    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );
 
    //wait 2 seconds
    SDL_Delay( 2000 );
 
    //Quit SDL (this also cleans up the SDL_Surface 
    //data - so never fear, we aren't leaking :)
    SDL_Quit();
 
    return 0;
}
