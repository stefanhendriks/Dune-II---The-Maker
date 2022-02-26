#include <SDL2/SDL.h>
 
#include <string>
#include <iostream>

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
    //SDL_Surface * surface = SDL_LoadBMP("test.bmp");                            // CPU memory

    char* bitmap = new char[1536276];
    // load into memory
    SDL_RWops *rwf = SDL_RWFromFile("test.pak","rb");
    if (rwf != NULL) {
        SDL_RWread(rwf, bitmap, 1536276, 1);
        SDL_RWclose(rwf);
        std::cout << "Load pak into memory" << std::endl;
    } else {
        std::cout << "Failed to load pak : " << SDL_GetError() << std::endl;
    }

    // read from memory
    SDL_RWops *rwm = SDL_RWFromMem(&bitmap[1228938], 307338);
    /* "rb" will "read binary" files */
    // SDL_RWops *file = SDL_RWFromFile("test.bmp", "rb");
    /* freesrc is true so the file automatically closes */
    SDL_Surface *surface = SDL_LoadBMP_RW(rwm, SDL_TRUE);
    if (!surface) {
        std::cout << "Failed to load image : " << SDL_GetError() << std::endl;
    }else {
        std::cout << "Load image" << std::endl;
    }

    delete bitmap;

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