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
const int minTileSize = 4;
const int maxTileSize = 64;
const int deltaTileSize = 4;

const int sBS = 96; // startBarSize
const int sBB = 5; // sizeBetweenButtons

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

    rectGui = cRectangle(sBS+1*(heightBarSize+sBB),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(SPICELAYER))
            .onClick([this]() {
                std::cout << "SPICELAYER" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(sBS+2*(heightBarSize+sBB),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(BUILDING))
            .onClick([this]() {
                std::cout << "BUILDING" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(sBS+3*(heightBarSize+sBB),1,heightBarSize,heightBarSize);
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
    auto rectGui = cRectangle(m_game.m_screenW-heightBarSize-1,sBS,heightBarSize,heightBarSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_HILL))
            .onClick([this]() {
                std::cout << "TERRAN_HILL" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1, sBS+heightBarSize+sBB,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_MOUNTAIN))
            .onClick([this]() {
                std::cout << "TERRAN_MOUNTAIN" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1,sBS+2*(heightBarSize+sBB),heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_ROCK))
            .onClick([this]() {
                std::cout << "TERRAN_ROCK" << std::endl;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_modifBar->addGuiObject(guiButton);
    
    rectGui = cRectangle(m_game.m_screenW-heightBarSize-1, sBS+3*(heightBarSize+sBB),heightBarSize,heightBarSize);
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
            tileLenSize -= deltaTileSize;
            tileLenSize = std::max(tileLenSize, minTileSize);
        } else if (event.eventType == MOUSE_SCROLLED_UP) {
            tileLenSize += deltaTileSize;
            tileLenSize = std::min(tileLenSize, maxTileSize);
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
            // Clamp camera to map bounds
            clampCameraXToMapBounds();
            clampCameraYToMapBounds();
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
        clampCameraXToMapBounds();
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_RIGHT)) {
        cameraX +=tileLenSize;
        clampCameraXToMapBounds();
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_UP)) {
        cameraY -=tileLenSize;
        clampCameraYToMapBounds();
    }
    if (event.isType(eKeyEventType::HOLD) && event.hasKey(SDL_Scancode::SDL_SCANCODE_DOWN)) {
        cameraY +=tileLenSize;
        clampCameraYToMapBounds();
    }
    if ((event.isType(eKeyEventType::HOLD) && (event.hasKeys(SDL_Scancode::SDL_SCANCODE_LSHIFT ,SDL_Scancode::SDL_SCANCODE_UP))
            || (event.isType(eKeyEventType::PRESSED) && event.hasKey(SDL_Scancode::SDL_SCANCODE_PAGEUP)))) {
        int prevTileLenSize = tileLenSize;
        tileLenSize += deltaTileSize;
        tileLenSize = std::min(tileLenSize, maxTileSize);
        // std::cout << "Zoom in, tile size from " << prevTileLenSize << " to " << tileLenSize << std::endl;
        int centerScreenX = mapSizeArea.getWidth() / 2;
        int centerScreenY = mapSizeArea.getWidth() / 2;
        int worldTileX = (cameraX + centerScreenX) / prevTileLenSize;
        int worldTileY = (cameraY + centerScreenY) / prevTileLenSize;
        // Adjust the camera to keep the same tile under the cursor
        cameraX = worldTileX * tileLenSize - centerScreenX;
        cameraY = worldTileY * tileLenSize - centerScreenY;
        clampCameraXToMapBounds();
        clampCameraYToMapBounds();
    }
    if ((event.isType(eKeyEventType::HOLD) && event.hasKeys(SDL_Scancode::SDL_SCANCODE_LSHIFT ,SDL_Scancode::SDL_SCANCODE_DOWN))
            || ((event.isType(eKeyEventType::PRESSED) && event.hasKey(SDL_Scancode::SDL_SCANCODE_PAGEDOWN)))) {
        int prevTileLenSize = tileLenSize;
        tileLenSize -= deltaTileSize;
        tileLenSize = std::max(tileLenSize, minTileSize);
        // std::cout << "Zoom out, tile size from " << prevTileLenSize << " to " << tileLenSize << std::endl;
        int centerScreenX = mapSizeArea.getWidth() / 2;
        int centerScreenY = mapSizeArea.getWidth() / 2;
        int worldTileX = (cameraX + centerScreenX) / prevTileLenSize;
        int worldTileY = (cameraY + centerScreenY) / prevTileLenSize;
        // Adjust the camera to keep the same tile under the cursor
        cameraX = worldTileX * tileLenSize - centerScreenX;
        cameraY = worldTileY * tileLenSize - centerScreenY;
        clampCameraXToMapBounds();
        clampCameraYToMapBounds();
    }
}

void cEditorState::loadMap(s_PreviewMap* map)
{
    std::cout << "open |"<< map->name << "|" << std::endl;
    m_mapData = std::make_unique<Matrix<int>>(map->terrainType, map->width, map->height);
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
    // Convert the camera position (in pixels) to tile coordinates
    int startX = cameraX / tileLenSize;
    int startY = cameraY / tileLenSize;

    // Calculating the number of tiles that fit on the screen (+1 to be sure of coverage)
    size_t tilesAcross = (mapSizeArea.getWidth() / tileLenSize) + 1;
    size_t tilesDown = (mapSizeArea.getHeight() / tileLenSize) + 1;

    // Determine the end tile
    size_t endX = startX + tilesAcross;
    size_t endY = startY + tilesDown;
    // Clamp to avoid wrong map m_mapData access
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
    for (size_t i = startX; i < endX; ++i) {
        for (size_t j = startY; j < endY; ++j) {
            
            // Ideal position in pixels if the map started at (0,0)
            tile_world_x = i * tileLenSize;
            tile_world_y = j * tileLenSize;
            // Rendering position on the screen (including camera offset)
            tile_screen_x = tile_world_x - cameraX;
            tile_screen_y = tile_world_y - cameraY;
            
            destRect= cRectangle(tile_screen_x, heightBarSize+tile_screen_y,tileLenSize,tileLenSize);
           
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