/**
 * @file Graphics.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "Graphics.hpp"
#include "utils/cDataPack.hpp"
#include "include/Texture.hpp"
#include <iostream>
#include "include/cAssert.h"
#include "utils/Log.h"

Graphics::Graphics(SDL_Renderer *_renderer,const std::string &filePackName): renderer(_renderer)
{
    d2tm_assert(_renderer != nullptr);
    dataPack = std::make_unique<DataPack>(filePackName, nullptr);
}

Graphics::~Graphics()
{
    // Smart pointers handle automatic cleanup
}

SDL_Surface *Graphics::getSurface(SurfaceID id) const
{
    return dataPack->getSurface(toPackIndex(id));
}

Texture *Graphics::getTexture(TextureID id)
{
    const int index = toPackIndex(id);
    if (index < 0) {
        std::cerr << "Graphics: Invalid index " << index << std::endl;
        return nullptr;
    }
    auto it = texCache.find(index);
    if (it != texCache.end())
        return it->second.get();

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
    SDL_SetTextureScaleMode(outTexture, SDL_SCALEMODE_NEAREST);
    texCache.emplace(index, std::make_unique<Texture>(outTexture, outSurface->w, outSurface->h));
    return texCache[index].get();
}

SurfaceID Graphics::getSurfaceID(std::string_view name) const
{
    const int index = dataPack->getIndexFromName(std::string(name));
    if (index < 0) {
        Logger::error(COMP_INIT, "Graphics", "image '{}' not found in the graphics pack", name);
        return SurfaceID::invalid;
    }
    return toSurfaceID(index);
}

TextureID Graphics::getTextureID(std::string_view name) const
{
    const int index = dataPack->getIndexFromName(std::string(name));
    if (index < 0) {
        Logger::error(COMP_INIT, "Graphics", "image '{}' not found in the graphics pack", name);
        return TextureID::invalid;
    }
    return toTextureID(index);
}

int Graphics::getNumberOfFiles()
{
    return dataPack->getNumberOfFile();
}

std::vector<std::string> Graphics::getNames() const
{
    return dataPack->getNames();
}
