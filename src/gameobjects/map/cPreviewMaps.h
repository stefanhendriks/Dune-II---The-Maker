/**
 * @file cPreviewMaps.h
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

// #define MAX_SKIRMISHMAPS 100     // max of 100 skirmish maps

#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <array>
#include <memory>

#include "gameobjects/map/PreviewMap.hpp"

class Texture;
class SDLDrawer;

// a preview map plus the SDL resources used to show it as a preview
struct PreviewMapView {
    s_PreviewMap map;
    SDL_Surface *terrain = nullptr;      // terrain bitmap (for showing preview at top right)
    Texture *previewTex = nullptr;       // texture for the map preview
};

class cPreviewMaps {
public:
    cPreviewMaps();

    void setSDLDrawer(SDLDrawer *sdlDrawer);

    // draws the terrain and start cells of a preview map onto a surface of the same size
    void drawPreview(const s_PreviewMap &previewMap, SDL_Surface *surface) const;

    ~cPreviewMaps() = default;

    void loadSkirmishMaps();
    void destroy();

    s_PreviewMap *getMap(int i);
    PreviewMapView *getView(int i);
    bool isValidMap(int i) const;

    std::string getMapSize(int i) const;
    int getMapCount() const;

    void createEmptyMap(const std::string &name, const std::string &author,
            const std::string &desciption, int width, int height);

    // replaces the map that loadMapFromEditor(-1) hands to the editor
    void setEditorMap(s_PreviewMap &&map);

    void loadSkirmish(const std::string &filename);
private:
    void initRandomMap();
    void initPreviews();
    std::unique_ptr<PreviewMapView> m_emptyMap;

    std::vector<std::unique_ptr<PreviewMapView>> m_PreviewMap;
    SDLDrawer * m_sdlDrawer = nullptr;
};