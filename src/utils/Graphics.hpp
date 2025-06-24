#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <unordered_map>

#include "include/Texture.hpp"

class DataPack;

/*
 * This class provides the Images of its internal container.
 * It only returns the Surfaces of the container.
 * It returns the Textures of the container. It manages its creation and destruction via a cache to avoid rebuilding the texture.
 */
class Graphics {
public:
    explicit Graphics(SDL_Renderer* _renderer, const std::string &filePackName);
    ~Graphics();
    // return a surface from his index
    SDL_Surface *getSurface(int index) const;
    // return a surface from his name    
    SDL_Surface *getSurface(const std::string &name) const;
    // return a texture from his index
    Texture *getTexture(int index);
    // return a texture from his name    
    Texture *getTexture(const std::string &name);
private:
    SDL_Renderer* renderer;
    std::unique_ptr<DataPack> dataPack;
    std::unordered_map<int , Texture*> texCache;
};