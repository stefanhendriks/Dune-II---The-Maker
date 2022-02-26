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

void writeHeader(SDL_RWops *file, Uint16 _nbFiles)
{
	const char *str = "D2TM";
    size_t len = SDL_strlen(str);
    if (SDL_RWwrite(file, str, 1, len) != len) {
        printf("Couldn't fully write string\n");
    }
    Uint16 nbFiles = _nbFiles;
    SDL_RWwrite(file, reinterpret_cast<const char*>(&nbFiles), sizeof(u_int16_t), 1);
}


void wrileFileLine(SDL_RWops *file, const char *name, uint32_t _offset, uint32_t _fileSize)
{
    char fileID[40]{'\0'};
    strcpy(fileID,name);
    if (SDL_RWwrite(file, fileID, 1, 40) != 40) {
        printf("Couldn't fully write string\n");
    }
    uint32_t offset = _offset;
    SDL_RWwrite(file, reinterpret_cast<const char*>(&offset), sizeof(uint32_t), 1);
    uint32_t fileSize = _fileSize;
    SDL_RWwrite(file, reinterpret_cast<const char*>(&fileSize), sizeof(uint32_t), 1); 
}


void copyFile(SDL_RWops *file, const char *fileName)
{
    SDL_RWops *wf = SDL_RWFromFile(fileName,"rb");
    Sint64 res_size = SDL_RWsize(wf);
    char* res = (char*)malloc(res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
        nb_read = SDL_RWread(wf, buf, 1, (res_size - nb_read_total));
        SDL_RWwrite(file,buf,1,(res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
}

int main(int argc, char ** argv)
{
if (1) {
	//
    // hand write pak file.
    //
	SDL_RWops *wf = SDL_RWFromFile("test1.pak","wb");
    //
    writeHeader(wf,3);
    //
    wrileFileLine(wf, "test1", 0, 1228938);
    wrileFileLine(wf, "test2", 1228938, 307338);
    wrileFileLine(wf, "test3", 1228938+307338, 691338);

    copyFile(wf,"test1.bmp");
    copyFile(wf,"test2.bmp");
    copyFile(wf,"test3.bmp");
    SDL_RWclose(wf);
    // hand write pak file.
}    
	
    bool quit = false;
    SDL_Event event;
 
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

    // load texture
    //SDL_Surface * surface = SDL_LoadBMP("test.bmp");                            // CPU memory

    char* bitmap = new char[1228938+307338+691338];
    // load into memory
    SDL_RWops *rwf = SDL_RWFromFile("test1.pak","rb");
    if (rwf != NULL) {
        SDL_RWseek(rwf,150,RW_SEEK_SET);
        SDL_RWread(rwf, bitmap, 2227614, 1);
        SDL_RWclose(rwf);
        std::cout << "Load pak into memory" << std::endl;
    } else {
        std::cout << "Failed to load pak : " << SDL_GetError() << std::endl;
    }

    // // read from memory
    SDL_RWops *rwm1 = SDL_RWFromMem(&bitmap[0], 1228938);
    SDL_RWops *rwm2 = SDL_RWFromMem(&bitmap[1228938], 307338);
    SDL_RWops *rwm3 = SDL_RWFromMem(&bitmap[1228938+307338], 691338);
    /* "rb" will "read binary" files */
    // SDL_RWops *file = SDL_RWFromFile("test.bmp", "rb");
    /* freesrc is true so the file automatically closes */
    SDL_Surface *surface1 = SDL_LoadBMP_RW(rwm1, SDL_TRUE);
    if (!surface1) {
    printf("Failed to load image 1 : %s\n", SDL_GetError());
    }
    SDL_Surface *surface2 = SDL_LoadBMP_RW(rwm2, SDL_TRUE);
    if (!surface2) {
    printf("Failed to load image 2 : %s\n", SDL_GetError());
    }
    SDL_Surface *surface3 = SDL_LoadBMP_RW(rwm3, SDL_TRUE);
    if (!surface3) {
    printf("Failed to load image 3 : %s\n", SDL_GetError());
    }
    delete bitmap;

    SDL_Texture * texture1 = SDL_CreateTextureFromSurface(renderer, surface1);    // GPU memory
    SDL_Texture * texture2 = SDL_CreateTextureFromSurface(renderer, surface2);    // GPU memory
    SDL_Texture * texture3 = SDL_CreateTextureFromSurface(renderer, surface3);    // GPU memory
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
    SDL_FreeSurface(surface3);

    SDL_Color color = { 255, 255, 255 };
    
    SDL_Texture * texture = texture3;

    while (!quit)
    {
        SDL_WaitEvent(&event);
 
        switch (event.type)
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_a){
                    texture = texture1;
		        }
                if (event.key.keysym.sym == SDLK_z){
                    texture = texture2;
		        }
                if (event.key.keysym.sym == SDLK_e){
                    texture = texture3;
		        }
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