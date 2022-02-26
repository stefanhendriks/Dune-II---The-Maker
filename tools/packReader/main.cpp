#include <SDL2/SDL.h>
 
#include <string>

class ReaderPack
{
public:
    ReaderPack(const std::string &filename){};
    ~ReaderPack(){};

    SDL_Surface* loadBitmap(int index);
    
private:
    bool readIndex();


};


int main(int argc, char ** argv)
{
    bool quit = false;
    SDL_Event event;
 
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

    // load texture
    SDL_Surface * surface = SDL_LoadBMP("test.bmp");                            // CPU memory
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);    // GPU memory
    SDL_FreeSurface(surface);

    SDL_Color color = { 255, 255, 255 };
 
    while (!quit)
    {
        SDL_WaitEvent(&event);
 
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
        }
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(5);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
 
    return 0;
}