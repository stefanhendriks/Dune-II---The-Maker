#include "cDataPack.hpp"
#include "cPack.h"
#include <iostream>
#include <iomanip>
#include <SDL3_image/SDL_image.h>

DataPack::DataPack(const std::string &packName, MIX_Mixer *mixer)
    : m_mixer(mixer)
{
    reader = std::make_unique<ReaderPack>(packName);
}

DataPack::~DataPack()
{
    for (auto& [_, surface] : surfaceCache) {
        if (surface) SDL_DestroySurface(surface);
    }
    for (auto& [_, audio] : audioCache) {
        if (audio) MIX_DestroyAudio(audio);
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

int DataPack::getIndexFromName(const std::string &name)
{
    return reader->getIndexFromName(name);
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
    SDL_IOStream *tmp = reader->getData(index);
    SDL_Surface *out = IMG_Load_IO(tmp, true);
    if (!out) {
        printf("Failed to load image %i : %s\n", index, SDL_GetError());
    }
    surfaceCache[index] = out;
    return out;
}

MIX_Audio *DataPack::getAudio(const std::string &name)
{
    int index = reader->getIndexFromName(name);
    return this->getAudio(index);
}

MIX_Audio *DataPack::getAudio(int index)
{
    if (index < 0 || index >= reader->getNumberOfFiles()) {
        std::cerr << "Invalid index: " << index << std::endl;
        return nullptr;
    }
    auto it = audioCache.find(index);
    if (it != audioCache.end())
        return it->second;
    SDL_IOStream *tmp = reader->getData(index);
    MIX_Audio *out = MIX_LoadAudio_IO(m_mixer, tmp, true, true);
    if (!out) {
        printf("Failed to load audio %i : %s\n", index, SDL_GetError());
    }
    audioCache[index] = out;
    return out;
}

int DataPack::getNumberOfFile() const
{
    return reader->getNumberOfFiles();
}
