#include "gamestates/cEditorState.h"
#include "gui/GuiBar.h"
#include "gui/GuiStateButton.h"
#include "gui/GuiButtonGroup.h"
#include "d2tmc.h"
#include "config.h"
#include "data/gfxinter.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/GraphicsContext.hpp"
#include "map/cPreviewMaps.h"
#include "utils/Graphics.hpp"
#include "data/gfxeditor.h"
#include "data/gfxinter.h"

#include <SDL2/SDL.h>
#include <format>
#include <iostream>

const int heightBarSize = 48;

cEditorState::cEditorState(cGame &theGame, GameContext* ctx) 
    : cGameState(theGame, ctx),
    m_gfxdata(ctx->getGraphicsContext()->gfxdata.get()),
    m_gfxeditor(ctx->getGraphicsContext()->gfxeditor.get())
{
    const cRectangle &selectRect = cRectangle(0, 0, m_game.m_screenW, heightBarSize+1);
    const cRectangle &modifRect = cRectangle(m_game.m_screenW-heightBarSize-1, heightBarSize-1, heightBarSize, m_game.m_screenH-heightBarSize);
    mapSizeArea = cRectangle(0,heightBarSize,m_game.m_screenW-heightBarSize-1,m_game.m_screenH-heightBarSize-1);
    m_selectBar = std::make_unique<GuiBar>(selectRect,GuiBarPlacement::HORIZONTAL);
    m_modifBar = std::make_unique<GuiBar>(modifRect,GuiBarPlacement::VERTICAL);
    m_selectBar->setTheme(GuiTheme::Light());
    m_modifBar->setTheme(GuiTheme::Light());

    populateSelectBar();
    populateModifBar();
    std::cout << "Entered Editor State" << std::endl;
}

cEditorState::~cEditorState()
{

}

void cEditorState::populateSelectBar()
{
    m_selectGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(96,1,heightBarSize,heightBarSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STONELAYER))
            .onClick([this]() {
                std::cout << "STONELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+1*(heightBarSize+5),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(SPICELAYER))
            .onClick([this]() {
                std::cout << "SPICELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+2*(heightBarSize+5),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(BUILDING))
            .onClick([this]() {
                std::cout << "BUILDING" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(96+3*(heightBarSize+5),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(UNITS))
            .onClick([this]() {
                std::cout << "UNITS" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);
}

void cEditorState::populateModifBar()
{
    m_topologyGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(m_game.m_screenW-heightBarSize-1,96,heightBarSize,heightBarSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_HILL))
            .onClick([this]() {
                std::cout << "TERRAN_HILL" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1, 96+heightBarSize+5,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_MOUNTAIN))
            .onClick([this]() {
                std::cout << "TERRAN_MOUNTAIN" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1,96+2*(heightBarSize+5),heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_ROCK))
            .onClick([this]() {
                std::cout << "TERRAN_ROCK" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1, 96+3*(heightBarSize+5),heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SAND))
            .onClick([this]() {
                std::cout << "TERRAN_SAND" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
}


void cEditorState::thinkFast()
{
}

void cEditorState::draw() const
{
    drawMap();
    m_selectBar->draw();
    m_modifBar->draw();
    m_game.getMouse()->draw();
}

void cEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.coords.isWithinRectangle(&mapSizeArea)) {
        // Zoom centered on the cursor
        int mouseX = event.coords.x;
        int mouseY = event.coords.y - mapSizeArea.getY(); // offset barre
        int prevTileLenSize = tileLenSize;
        if (event.eventType == MOUSE_SCROLLED_DOWN) {
            tileLenSize -= 4;
            tileLenSize = std::max(tileLenSize, 4);
        } else if (event.eventType == MOUSE_SCROLLED_UP) {
            tileLenSize += 4;
            tileLenSize = std::min(tileLenSize, 64);
        } else {
            return;
        }
        // Zoom change
        if (tileLenSize != prevTileLenSize) {
            // Calculate the tile under the cursor before zooming
            int worldTileX = (cameraX + mouseX) / prevTileLenSize;
            int worldTileY = (cameraY + mouseY) / prevTileLenSize;
            // Adjust the camera to keep the same tile under the cursor
            cameraX = worldTileX * tileLenSize - mouseX;
            cameraY = worldTileY * tileLenSize - mouseY;
            // Clamp caméra pour ne pas sortir de la carte
            clampCameraXToMapBounds();
            clampCameraYToMapBounds();
            // int maxCameraX = m_mapData ? (m_mapData->getRows() * tileLenSize - mapSizeArea.getWidth()) : 0;
            // int maxCameraY = m_mapData ? (m_mapData->getCols() * tileLenSize - mapSizeArea.getHeight()) : 0;
            // if (cameraX < 0) cameraX = 0;
            // if (cameraY < 0) cameraY = 0;
            // if (m_mapData) {
            //     if (maxCameraX < 0) maxCameraX = 0;
            //     if (maxCameraY < 0) maxCameraY = 0;
            //     if (cameraX > maxCameraX) cameraX = maxCameraX;
            //     if (cameraY > maxCameraY) cameraY = maxCameraY;
            // }
        }
        return;
    } else {
        m_selectBar->onNotifyMouseEvent(event);
        m_modifBar->onNotifyMouseEvent(event);
    }
}

eGameStateType cEditorState::getType()
{
    return GAMESTATE_EDITOR;
}

void cEditorState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(SDL_SCANCODE_ESCAPE)) {
        m_game.setNextStateToTransitionTo(GAME_MENU);
        m_game.initiateFadingOut();
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_LEFT)) {
        cameraX -=tileLenSize;
        std::cout << "CameraX: " << cameraX << std::endl;
        std::cout << "MapSizeX: " << mapSizeArea.getWidth() << std::endl;
        std::cout << "MaxSizeX: " << m_mapData->getRows()*tileLenSize << std::endl;

        if (cameraX <0) {
            cameraX =0;
            std::cout << "CameraX CORRECT: " << cameraX << std::endl;
        }
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_RIGHT)) {
        cameraX +=tileLenSize;
        std::cout << "CameraX: " << cameraX << std::endl;
        std::cout << "MapSizeX: " << mapSizeArea.getWidth() << std::endl;
        std::cout << "MaxSizeX: " << m_mapData->getRows()*tileLenSize << std::endl;

        clampCameraXToMapBounds();
        // if (cameraX > (m_mapData->getRows()*tileLenSize - mapSizeArea.getWidth())) {
        //     cameraX = m_mapData->getRows()*tileLenSize - mapSizeArea.getWidth();
        //     std::cout << "CameraX CORRECT: " << cameraX << std::endl;
        // }
        // if (m_mapData->getRows()*tileLenSize< mapSizeArea.getWidth()) {
        //     cameraX =0;
        // }
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_UP)) {
        cameraY -=tileLenSize;
        if (cameraY <0) {
            cameraY =0;
        }
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_DOWN)) {
        cameraY +=tileLenSize;
        clampCameraYToMapBounds();
        // if (cameraY > (m_mapData->getCols()*tileLenSize - mapSizeArea.getHeight())) {
        //     cameraY = m_mapData->getCols()*tileLenSize - mapSizeArea.getHeight();
        // }
        // if (m_mapData->getCols()*tileLenSize< mapSizeArea.getHeight()) {
        //     cameraY =0;
        // }
    }
}

void cEditorState::loadMap(s_PreviewMap* map)
{
    std::cout << "open |"<< map->name << "|" << std::endl;
    m_mapData = std::make_unique<Matrix<int>>(map->terrainType, map->width, map->height);
}

void cEditorState::clampCameraToMapBounds()
{
    if (m_mapData == nullptr) {
        cameraX = 0;
        cameraY = 0;
        return;
    }
    int maxCameraX = m_mapData->getRows() * tileLenSize - mapSizeArea.getWidth();
    int maxCameraY = m_mapData->getCols() * tileLenSize - mapSizeArea.getHeight();

    if (cameraX < 0) cameraX = 0;
    if (cameraY < 0) cameraY = 0;
    if (maxCameraX < 0) maxCameraX = 0;
    if (maxCameraY < 0) maxCameraY = 0;
    if (cameraX > maxCameraX) cameraX = maxCameraX;
    if (cameraY > maxCameraY) cameraY = maxCameraY;
}

void cEditorState::clampCameraYToMapBounds()
{
    if (m_mapData == nullptr) {
        cameraY = 0;
        return;
    }
    int maxCameraY = m_mapData->getCols() * tileLenSize - mapSizeArea.getHeight();
    if (cameraY < 0) cameraY = 0;
    if (maxCameraY < 0) maxCameraY = 0;
    if (cameraY > maxCameraY) cameraY = maxCameraY;
}

void cEditorState::clampCameraXToMapBounds()
{
    if (m_mapData == nullptr) {
        cameraX = 0;
        return;
    }
    int maxCameraX = m_mapData->getRows() * tileLenSize - mapSizeArea.getWidth();
    if (cameraX < 0) cameraX = 0;
    if (maxCameraX < 0) maxCameraX = 0;
    if (cameraX > maxCameraX) cameraX = maxCameraX;
}

void cEditorState::drawMap() const
{
    if (m_mapData == nullptr) {
        return;
    }
    // 1. Convertir la position de la caméra (en pixels) en coordonnées de tuiles
    int startX = cameraX / tileLenSize;
    int startY = cameraY / tileLenSize;

    // 2. Calcul due nombre de tuiles qui tiennent sur l'écran (+1 pour être sûr de couvrir)
    size_t tilesAcross = (mapSizeArea.getWidth() / tileLenSize) + 1;
    size_t tilesDown = (mapSizeArea.getHeight() / tileLenSize) + 1;
    // std::cout << "Tiles across: " << tilesAcross << " Tiles down: " << tilesDown << " tileLenSize: " << tileLenSize << std::endl;

    // 3. Détermine la tuile de fin
    size_t endX = startX + tilesAcross;
    size_t endY = startY + tilesDown;
    // std::cout << "Bx: "<< startX << " tileX: " << tilesAcross << " Ex: "<< endX << " Mx: " << m_mapData->getRows()<<std::endl;
    // std::cout << "By: "<< startY << " tileY: " << tilesDown << " Et: "<< endY << " My: " << m_mapData->getCols()<<std::endl;
    // 4. Clamper pour ne pas sortir des limites de la carte totale
    if (endX > m_mapData->getRows()) {
        endX = m_mapData->getRows();
        startX = endX - tilesAcross;
        if (startX < 0) {
            startX = 0; 
        }
    }
    if (endY > m_mapData->getCols()) {
        endY = m_mapData->getCols();
        startY = endY - tilesDown;
        if (startY < 0) {
            startY = 0; 
        }
    }
    
    int tileID;
    int tile_world_x, tile_world_y;
    int tile_screen_x, tile_screen_y;

    cRectangle destRect;
    cRectangle srcRect{0,0,32,32}; // we take the first full textured sprite
    for (size_t i = startX; i < endX; ++i) { // Lignes
        for (size_t j = startY; j < endY; ++j) { // Colonnes
            
            // 1. Position idéale en pixels si la carte démarrait à (0,0)
            tile_world_x = i * tileLenSize;
            tile_world_y = j * tileLenSize;

            // 2. Position de rendu sur l'écran (avec le décalage de la caméra)
            tile_screen_x = tile_world_x - cameraX;
            tile_screen_y = tile_world_y - cameraY;
            
            destRect= cRectangle(tile_screen_x, 48+tile_screen_y,tileLenSize,tileLenSize);
           
            tileID = (*m_mapData)[i][j];
            switch (tileID)
            {
            case TERRAIN_SPICE:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SPICE), srcRect, destRect);
                break;
            case TERRAIN_SAND:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SAND), srcRect, destRect);
                break;
            case TERRAIN_MOUNTAIN:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_MOUNTAIN), srcRect, destRect);
                break;
            case TERRAIN_ROCK:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_ROCK), srcRect, destRect);
                break;
            case TERRAIN_SPICEHILL:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SPICEHILL), srcRect, destRect);
                break;  
            case TERRAIN_HILL:
                renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_HILL), srcRect, destRect);
                break;            
            default:
                break;
            }
        }
    }
}