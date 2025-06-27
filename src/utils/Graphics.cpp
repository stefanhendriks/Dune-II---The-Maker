#include "Graphics.hpp"
#include "DataPack.hpp"
#include <iostream>

Graphics::Graphics(SDL_Renderer *_renderer,const std::string &filePackName): renderer(_renderer)
{
    dataPack = std::make_unique<DataPack>(filePackName);
}

Graphics::~Graphics()
{
    for (auto& [_, texObj] : texCache) {
        // auto delete with smartptr required
        if (texObj) {
            SDL_DestroyTexture(texObj->tex);
            delete(texObj);
        }
    }
    texCache.clear();
}

SDL_Surface *Graphics::getSurface(int index) const
{
    return dataPack->getSurface(index);
}

SDL_Surface *Graphics::getSurface(const std::string &name) const
{
    return dataPack->getSurface(name);
}

Texture *Graphics::getTexture(int index)
{
    if (index < 0) {
        std::cerr << "Graphics: Invalid index " << index << std::endl;
        return nullptr;
    }
    auto it = texCache.find(index);
    if (it != texCache.end())
        return it->second;

    SDL_Surface *outSurface = dataPack->getSurface(index);
    if (!outSurface) {
        std::cerr << "Graphics: Failed to load image " << index << " : " <<SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture *outTexture = SDL_CreateTextureFromSurface(renderer, outSurface);
    if (!outTexture) {
        std::cerr << "Graphics: Failed to convert texture " << index << " : " <<SDL_GetError() << std::endl;
        return nullptr;
    }
    Texture *Tex = new Texture(outTexture, outSurface->w, outSurface->h);
    texCache[index] = Tex;
    return Tex;
}

Texture *Graphics::getTexture(const std::string &name)
{
    int index = dataPack->getIndexFromName(name);
    return this->getTexture(index);
}

int Graphics::getNumberOfFiles()
{
    return dataPack->getNumberOfFile();
}