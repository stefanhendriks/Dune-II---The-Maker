/**
 * @file ContextCreator.hpp
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
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>

class cFileValidator;
class GraphicsContext;
class Graphics;
class TextContext;
class cTextDrawer;
class cGameSettings;
class SDLDrawer;

class ContextCreator {
public:
    ContextCreator(SDL_Renderer *renderer, cFileValidator *settingsValidator);
    ContextCreator() = delete;
    ~ContextCreator();

    std::unique_ptr<GraphicsContext> createGraphicsContext();
    std::unique_ptr<TextContext> createTextContext(cGameSettings *settings, SDLDrawer *renderer);

private:
    std::shared_ptr<Graphics> gfxmentat;
    std::shared_ptr<Graphics> gfxworld;
    std::shared_ptr<Graphics> gfxinter;
    std::shared_ptr<Graphics> gfxdata;
    std::shared_ptr<Graphics> gfxeditor;

    TTF_Font *game_font = nullptr; 	// arrakeen.fon
    TTF_Font *bene_font = nullptr;	// benegesserit font.
    TTF_Font *small_font = nullptr;	// small font.
};