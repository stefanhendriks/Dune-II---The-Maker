#include "DataPack.hpp"
#include "pack.h"
#include <iostream>
#include <iomanip>

DataPack::DataPack(const std::string &packName)
{
    reader = std::make_unique<ReaderPack>(packName);
}

DataPack::~DataPack()
{
    for (auto& [_, texture] : surfaceCache) {
        if (texture) SDL_FreeSurface(texture);
    }
    for (auto& [_, music] : musicCache) {
        if (music) Mix_FreeMusic(music);
    }
    for (auto& [_, chunk] : sampleCache) {
        if (chunk) Mix_FreeChunk(chunk);
    }
    reader.reset();
}

void DataPack::displayPackFile()
{
    reader->displayPackFile();
}

SDL_Surface *DataPack::getSurface(const std::string &name)
{
    int index = reader->getIndexFromName(name);
    return this->getSurface(index);
}

SDL_Surface *DataPack::getSurface(int index)
{
    if (index < 0 || index >= reader->getNumberOfFiles()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    auto it = surfaceCache.find(index);
    if (it != surfaceCache.end())
        return it->second;
    SDL_RWops *tmp = reader->getData(index);
    SDL_Surface *out = SDL_LoadBMP_RW(tmp, SDL_TRUE);
    if (!out) {
        printf("Failed to load image %i : %s\n", index, SDL_GetError());
    }
    surfaceCache[index] = out;
    return out;
}

Mix_Music *DataPack::getMusic(const std::string &name)
{
    int index = reader->getIndexFromName(name);
    return this->getMusic(index);
}

Mix_Music *DataPack::getMusic(int index)
{
    if (index < 0 || index >= reader->getNumberOfFiles()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    auto it = musicCache.find(index);
    if (it != musicCache.end())
        return it->second;
    SDL_RWops *tmp = reader->getData(index);
    Mix_Music *out = Mix_LoadMUS_RW(tmp, SDL_TRUE);
    if (!out) {
        printf("Failed to load music %i : %s\n", index, SDL_GetError());
    }
    musicCache[index] = out;
    return out;
}

Mix_Chunk *DataPack::getSample(const std::string &name)
{
    int index = reader->getIndexFromName(name);
    return this->getSample(index);
}

Mix_Chunk *DataPack::getSample(int index)
{
    if (index < 0 || index >= reader->getNumberOfFiles()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    auto it = sampleCache.find(index);
    if (it != sampleCache.end())
        return it->second;
    SDL_RWops *tmp = reader->getData(index);
    Mix_Chunk *out = Mix_LoadWAV_RW(tmp, SDL_TRUE);
    if (!out) {
        printf("Failed to load sample %i : %s\n", index, SDL_GetError());
    }
    sampleCache[index] = out;
    return out;
}

int DataPack::getNumberOfFile() const
{
    return reader->getNumberOfFiles();
}

/*
 * This is a test program to read and write pack files.
 * It uses SDL2 and SDL2_mixer to display images and play music.
 * The pack files are created with WriterPack and read with DataPack.
 */
/*
int main(int argc, char ** argv)
{
    // First we create a pak file from scratch
    if (1) {
        // write pak file.
    	WriterPack test("test1.pak");
        //
        test.addFile("test1.bmp","test1");
        test.addFile("test2.bmp","test2");
        test.addFile("test3.bmp","test3");
        //
        test.writePackFilesOnDisk();
    }

    if (1) {
        // write pak file with audio file.
    	WriterPack test("audio1.pak");
        //
        test.addFile("DUNE1_4.mid","audio1");
        test.addFile("DUNE1_5.mid","audio2");
        test.addFile("DUNE1_6.mid","audio3");
        test.addFile("DUNE1_7.mid","audio4");
        //
        test.writePackFilesOnDisk();
    }

    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    //Initialize SDL2_mixer to play sound
    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        printf("SDL2_mixer could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_Window * window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

    //-----------------------------------------------------------------
    //load with DataPack
    //-----------------------------------------------------------------
    DataPack dataRead("test1.pak");
    SDL_Surface *surface1 = dataRead.getSurface(0);
    SDL_Surface *surface2 = dataRead.getSurface("test2");
    SDL_Surface *surface3 = dataRead.getSurface(2);
    dataRead.displayPackFile();
    //-----------------------------------------------------------------

    //convert SDL_Surface to SDL_Texture
    SDL_Texture * texture1 = SDL_CreateTextureFromSurface(renderer, surface1);    // GPU memory
    SDL_Texture * texture2 = SDL_CreateTextureFromSurface(renderer, surface2);    // GPU memory
    SDL_Texture * texture3 = SDL_CreateTextureFromSurface(renderer, surface3);    // GPU memory
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
    SDL_FreeSurface(surface3);

    SDL_Color color = { 255, 255, 255 };

    SDL_Texture * texture = texture1;

    //-----------------------------------------------------------------
    //load with DataPack and play it
    //-----------------------------------------------------------------
    DataPack dataAudio("audio1.pak");
    Mix_Music * music = dataAudio.getMusic(2);
    Mix_PlayMusic( music, -1 );
    dataAudio.displayPackFile();

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
                if (event.key.keysym.sym == SDLK_q){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio1");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_s){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio2");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_d){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio3");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_f){
                    Mix_FreeMusic(music);
                    music = dataAudio.getMusic("audio4");
                    Mix_PlayMusic( music, -1 );
		        }
                if (event.key.keysym.sym == SDLK_ESCAPE){
                    quit = true;
		        }
                break;
        }
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic();
    Mix_FreeMusic(music);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
*/