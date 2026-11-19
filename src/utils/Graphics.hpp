/**
 * @file Graphics.hpp
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

#pragma once

#include "utils/GraphicsID.hpp"

#include <SDL3/SDL.h>
#include <string>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <vector>

class Texture;

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
    // return a surface from his id
    SDL_Surface *getSurface(SurfaceID id) const;
    // return a texture from his id
    Texture *getTexture(TextureID id);
    // return a texture of a frame sequence (movie pack), unnamed: frames are numbered from 0 in pack order
    Texture *getFrameTexture(int frameNumber) { return getTexture(toTextureID(frameNumber)); }
    // resolve a name to an id once (at init) and keep the id; logs an error and returns invalid when the name is not in the pack
    SurfaceID getSurfaceID(std::string_view name) const;
    TextureID getTextureID(std::string_view name) const;
    // return number of file in Pack
    int getNumberOfFiles();
    // name of every file in the pack, in index order
    std::vector<std::string> getNames() const;
private:
    SDL_Renderer *renderer;
    std::unique_ptr<DataPack> dataPack;
    std::unordered_map<int, std::unique_ptr<Texture>> texCache;
};