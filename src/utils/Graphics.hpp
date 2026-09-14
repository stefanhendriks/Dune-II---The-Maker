/**
 * @file Graphics.hpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <SDL3/SDL.h>
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
    explicit Graphics(SDL_Renderer *_renderer, const std::string &filePackName);
    ~Graphics();
    // return a surface from his index
    SDL_Surface *getSurface(int index) const;
    // return a surface from his name
    SDL_Surface *getSurface(const std::string &name) const;
    // return a texture from his index
    Texture *getTexture(int index);
    // return a texture from his name
    Texture *getTexture(const std::string &name);
    // return number of file in Pack
    int getNumberOfFiles();
private:
    SDL_Renderer *renderer;
    std::unique_ptr<DataPack> dataPack;
    std::unordered_map<int, std::unique_ptr<Texture>> texCache;
};