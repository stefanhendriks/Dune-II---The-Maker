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
#include <fstream>

const int heightBarSize = 48;
const int heightButtonSize = 40;
const int halfMarginBetweenButtons = (heightBarSize-heightButtonSize)/2;
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
    const cRectangle &selectRect = cRectangle(0, 0, m_game.m_screenW, heightBarSize);
    const cRectangle &modifRect = cRectangle(m_game.m_screenW-heightBarSize, heightBarSize, heightBarSize, m_game.m_screenH-heightBarSize);
    mapSizeArea = cRectangle(0,heightBarSize,m_game.m_screenW-heightBarSize,m_game.m_screenH-heightBarSize);
    m_selectBar = std::make_unique<GuiBar>(selectRect,GuiBarPlacement::HORIZONTAL);
    m_topologyBar = std::make_unique<GuiBar>(modifRect,GuiBarPlacement::VERTICAL);
    m_startCellBar = std::make_unique<GuiBar>(modifRect,GuiBarPlacement::VERTICAL);
    m_selectBar->setTheme(GuiTheme::Light());
    m_topologyBar->setTheme(GuiTheme::Light());
    m_startCellBar->setTheme(GuiTheme::Light());

    populateSelectBar();
    populateTopologyBar();
    populateStartCellBar();
    startCells.fill({-1, -1});
    //std::cout << "Entered Editor State" << std::endl;
}

cEditorState::~cEditorState()
{

}

void cEditorState::populateSelectBar()
{
    m_selectGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(96,halfMarginBetweenButtons,heightButtonSize,heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STONELAYER))
            .onClick([this]() {
                // std::cout << "STONELAYER" << std::endl;
                m_currentBar = m_topologyBar.get();
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    guiButton->setPressed(true);
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(sBS+1*(heightBarSize+sBB),halfMarginBetweenButtons,heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION))
            .onClick([this]() {
                // std::cout << "STARTPOSITION" << std::endl;
                m_currentBar = m_startCellBar.get();
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

/*
    rectGui = cRectangle(sBS+2*(heightBarSize+sBB),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(SOLDIERS))
            .onClick([this]() {
                std::cout << "SOLDIERS" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(sBS+3*(heightBarSize+sBB),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(BUILDING))
            .onClick([this]() {
                std::cout << "BUILDING" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);

    rectGui = cRectangle(sBS+4*(heightBarSize+sBB),1,heightBarSize,heightBarSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(UNITS))
            .onClick([this]() {
                std::cout << "UNITS" << std::endl;
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addGuiObject(guiButton);
*/
}

void cEditorState::populateTopologyBar()
{
    m_topologyGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS,heightButtonSize,heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_HILL))
            .onClick([this]() {
                // std::cout << "TERRAN_HILL" << std::endl;
                idTerrainToMapModif = TERRAIN_HILL;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons, sBS+heightBarSize+sBB,heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_MOUNTAIN))
            .onClick([this]() {
                // std::cout << "TERRAN_MOUNTAIN" << std::endl;
                idTerrainToMapModif = TERRAIN_MOUNTAIN;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+2*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_ROCK))
            .onClick([this]() {
                // std::cout << "TERRAN_ROCK" << std::endl;
                idTerrainToMapModif = TERRAIN_ROCK;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons, sBS+3*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SAND))
            .onClick([this]() {
                // std::cout << "TERRAN_SAND" << std::endl;
                idTerrainToMapModif = TERRAIN_SAND;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addGuiObject(guiButton);
    guiButton->setPressed(true);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+4*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SPICE))
            .onClick([this]() {
                // std::cout << "TERRAN_SPICE" << std::endl;
                idTerrainToMapModif = TERRAIN_SPICE;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+5*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SPICEHILL))
            .onClick([this]() {
                // std::cout << "TERRAN_SPICEHILL" << std::endl;
                idTerrainToMapModif = TERRAIN_SPICEHILL;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addGuiObject(guiButton);
}

void cEditorState::populateStartCellBar()
{
    m_startCellGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS,heightButtonSize,heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION1))
            .onClick([this]() {
                // std::cout << "STARTPOSITION1" << std::endl;
                idStartCellPlayer = 0;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    guiButton->setPressed(true);
    m_startCellBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+1*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION2))
            .onClick([this]() {
                // std::cout << "STARTPOSITION2" << std::endl;
                idStartCellPlayer = 1;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+2*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION3))
            .onClick([this]() {
                // std::cout << "STARTPOSITION3" << std::endl;
                idStartCellPlayer = 2;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+3*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION4))
            .onClick([this]() {
                // std::cout << "STARTPOSITION4" << std::endl;
                idStartCellPlayer = 3;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addGuiObject(guiButton);

    rectGui = cRectangle(m_game.m_screenW-heightBarSize+halfMarginBetweenButtons,sBS+4*(heightBarSize+sBB),heightButtonSize,heightButtonSize);
    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION5))
            .onClick([this]() {
                // std::cout << "STARTPOSITION5" << std::endl;
                idStartCellPlayer = 4;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addGuiObject(guiButton);
}

void cEditorState::thinkFast()
{
}

void cEditorState::draw() const
{
    drawMap();
    if (m_currentBar == m_startCellBar.get())
        drawStartCells();
    m_selectBar->draw();
    m_currentBar->draw();
    m_game.getMouse()->draw();
}

void cEditorState::zoomAtMapPosition(int screenX, int screenY, ZoomDirection direction)
{
    int prevTileLenSize = tileLenSize;
    if (direction == ZoomDirection::zoomOut) {
        tileLenSize -= deltaTileSize;
        tileLenSize = std::max(tileLenSize, minTileSize);
    } else if (direction == ZoomDirection::zoomIn) {
        tileLenSize += deltaTileSize;
        tileLenSize = std::min(tileLenSize, maxTileSize);
    }
    // Zoom change
    if (tileLenSize != prevTileLenSize) {
        // Calculate the tile under the cursor before zooming
        int worldTileX = (cameraX + screenX) / prevTileLenSize;
        int worldTileY = (cameraY + screenY) / prevTileLenSize;
        // Adjust the camera to keep the same tile under the cursor
        cameraX = worldTileX * tileLenSize - screenX;
        cameraY = worldTileY * tileLenSize - screenY;
        // Clamp camera to map bounds
        clampCameraXToMapBounds();
        clampCameraYToMapBounds();
    }
}

void cEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_mapData == nullptr) {
        return;
    }
    if (event.coords.isWithinRectangle(&mapSizeArea)) {
        // Zoom centered on the cursor
        int mouseX = event.coords.x;
        int mouseY = event.coords.y - mapSizeArea.getY(); // offset barre
        if (event.eventType == MOUSE_SCROLLED_DOWN) {
            zoomAtMapPosition(mouseX, mouseY, ZoomDirection::zoomOut);
        } else if (event.eventType == MOUSE_SCROLLED_UP) {
            zoomAtMapPosition(mouseX, mouseY, ZoomDirection::zoomIn);
        } else if (event.eventType == MOUSE_LEFT_BUTTON_PRESSED && m_currentBar == m_topologyBar.get()) {
            modifyTile(mouseX, mouseY, idTerrainToMapModif);
        }else if (event.eventType == MOUSE_LEFT_BUTTON_PRESSED && m_currentBar == m_startCellBar.get()) {
            modifyStartCell(mouseX, mouseY, idStartCellPlayer);
        }        
        else {
            return;
        }
        updateVisibleTiles();
    } else {
        m_selectBar->onNotifyMouseEvent(event);
        m_currentBar->onNotifyMouseEvent(event);
    }
}

eGameStateType cEditorState::getType()
{
    return GAMESTATE_EDITOR;
}

void cEditorState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (m_mapData == nullptr) {
        return;
    }
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.hasKey(SDL_SCANCODE_ESCAPE)) {
            m_game.setNextStateToTransitionTo(GAME_MENU);
            m_game.initiateFadingOut();
        }
        if (event.hasKey(SDL_SCANCODE_S)) {
            //std::cout << "Save" << std::endl;
            saveMap();
        }
        if (event.hasKey(SDL_Scancode::SDL_SCANCODE_PAGEUP)) {
            zoomAtMapPosition(m_game.m_screenW/2, m_game.m_screenH/2, ZoomDirection::zoomIn);
        }
        if (event.hasKey(SDL_Scancode::SDL_SCANCODE_PAGEDOWN)) {
            zoomAtMapPosition(m_game.m_screenW/2, m_game.m_screenH/2, ZoomDirection::zoomOut);
        }
        updateVisibleTiles();
    }

    if (event.isType(eKeyEventType::HOLD)) {
        if (event.hasKey(SDL_Scancode::SDL_SCANCODE_LEFT)) {
            cameraX -=tileLenSize;
            clampCameraXToMapBounds();
        }
        if (event.hasKey(SDL_Scancode::SDL_SCANCODE_RIGHT)) {
            cameraX +=tileLenSize;
            clampCameraXToMapBounds();
        }
        if (event.hasKey(SDL_Scancode::SDL_SCANCODE_UP)) {
            cameraY -=tileLenSize;
            clampCameraYToMapBounds();
        }
        if (event.hasKey(SDL_Scancode::SDL_SCANCODE_DOWN)) {
            cameraY +=tileLenSize;
            clampCameraYToMapBounds();
        }
        if (event.hasKeys(SDL_Scancode::SDL_SCANCODE_LSHIFT ,SDL_Scancode::SDL_SCANCODE_UP)) {
            zoomAtMapPosition(m_game.m_screenW/2, m_game.m_screenH/2, ZoomDirection::zoomIn);
        }
        if (event.hasKeys(SDL_Scancode::SDL_SCANCODE_LSHIFT ,SDL_Scancode::SDL_SCANCODE_DOWN)) {
            zoomAtMapPosition(m_game.m_screenW/2, m_game.m_screenH/2, ZoomDirection::zoomOut);
        }
        updateVisibleTiles(); 
    }
}

void cEditorState::loadMap(s_PreviewMap* map)
{
    //std::cout << "open |"<< map->name << "|" << std::endl;
    m_mapData = std::make_unique<Matrix<int>>(map->terrainType, map->height, map->width);
    for (int i=0; i<MAX_SKIRMISHMAP_PLAYERS; i++) {
        if (map->iStartCell[i] !=-1) {
            //std::cout << "startCell " << map->iStartCell[i] << std::endl;
            int w = map->iStartCell[i] % map->width;
            int h = map->iStartCell[i] / map->width;
            startCells[i] = cPoint(w,h);
        }
    }
    updateVisibleTiles();
}

void cEditorState::clampCameraYToMapBounds()
{
    if (m_mapData == nullptr) {
        cameraY = 0;
        return;
    }
    if (cameraY < 0) {
        cameraY = 0;
        return;
    }
    int maxCameraY = m_mapData->getCols() * tileLenSize - mapSizeArea.getHeight();
    if (maxCameraY < 0) maxCameraY = 0;
    if (cameraY > maxCameraY) cameraY = maxCameraY;
}

void cEditorState::clampCameraXToMapBounds()
{
    if (m_mapData == nullptr) {
        cameraX = 0;
        return;
    }
    if (cameraX < 0) {
        cameraX = 0;
        return;
    }
    int maxCameraX = m_mapData->getRows() * tileLenSize - mapSizeArea.getWidth();
    if (maxCameraX < 0) maxCameraX = 0;
    if (cameraX > maxCameraX) cameraX = maxCameraX;
}

void cEditorState::drawMap() const
{
    if (m_mapData == nullptr) {
        return;
    }
    
    int tileID;
    int tile_world_x, tile_world_y;
    int tile_screen_x, tile_screen_y;

    cRectangle destRect;
    cRectangle srcRect{0,0,32,32}; // we take the first full textured sprite
    for (size_t j = startY; j < endY; j++) {
        for (size_t i = startX; i < endX; i++) {
            // Ideal position in pixels if the map started at (0,0)
            tile_world_x = i * tileLenSize;
            tile_world_y = j * tileLenSize;
            // Rendering position on the screen (including camera offset)
            tile_screen_x = tile_world_x - cameraX;
            tile_screen_y = tile_world_y - cameraY;
            
            destRect= cRectangle(tile_screen_x, heightBarSize+tile_screen_y,tileLenSize,tileLenSize);
           
            tileID = (*m_mapData)[j][i];
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

void cEditorState::updateVisibleTiles()
{
    // Convert the camera position (in pixels) to tile coordinates
    startX = cameraX / tileLenSize;
    startY = cameraY / tileLenSize;

    // Calculating the number of tiles that fit on the screen (+1 to be sure of coverage)
    tilesAcross = (mapSizeArea.getWidth() / tileLenSize) + 1;
    tilesDown = (mapSizeArea.getHeight() / tileLenSize) + 1;

    // Determine the end tile
    endX = startX + tilesAcross;
    endY = startY + tilesDown;
    // Clamp to avoid wrong map m_mapData access
    if (endX > m_mapData->getCols()) {
        endX = m_mapData->getCols();
        startX = endX - tilesAcross;
        if (startX < 0) {
            startX = 0; 
        }
    }
    if (endY > m_mapData->getRows()) {
        endY = m_mapData->getRows();
        startY = endY - tilesDown;
        if (startY < 0) {
            startY = 0; 
        }
    }
}

void cEditorState::modifyTile(int posX, int posY, int tileID)
{
    if (tileID == -1) {
        return;
    }
    int tileX = (cameraX + posX) / tileLenSize;
    int tileY = (cameraY + posY) / tileLenSize;
    if (m_mapData && tileX >= 0 && tileY >= 0 && tileX < (int)m_mapData->getRows() && tileY < (int)m_mapData->getCols()) {
        (*m_mapData)[tileY][tileX] = tileID;
    }
}

void cEditorState::modifyStartCell(int posX, int posY, int startCellID)
{
    if (startCellID == -1) {
        return;
    }
    int tileX = (cameraX + posX) / tileLenSize;
    int tileY = (cameraY + posY) / tileLenSize;
    if (m_mapData && tileX >= 1 && tileY >= 1 && tileX < (int)m_mapData->getRows()-1 && tileY < (int)m_mapData->getCols()-1) {
        startCells[startCellID]={tileX, tileY};
    }
}

void cEditorState::drawStartCells() const
{
    cRectangle destRect;
    cRectangle srcRect{0,0,32,32}; // we take the first full textured sprite
    int x,y;
    for(size_t i=0; i<startCells.size(); i++) {
        if (startCells[i].x != -1 && startCells[i].y != -1) {
            x = startCells[i].x * tileLenSize - cameraX;
            y = heightBarSize + startCells[i].y * tileLenSize - cameraY;
            // Display if onscreen
            if (x + tileLenSize > 0 && x < m_game.m_screenW &&y + tileLenSize > heightBarSize && y < m_game.m_screenH) {
                destRect = cRectangle(x, y, tileLenSize, tileLenSize);
                renderDrawer->renderStrechSprite(m_gfxeditor->getTexture(STARTPOSITION1+i), srcRect, destRect);
            }
        }
    }
}

void cEditorState::saveMap() const
{
    // creating file
    std::ofstream saveFile("skirmish/custom_map.ini");
    // file verification
    if (!saveFile.is_open()){
        std::cerr << "unable to open modified map for saving" << std::endl;
        return;
    }
    // map card
    saveFile << "[SKIRMISH]\nTitle='Custom map'\n";
    saveFile << "Author = -\nDescription = -\n";
    saveFile << "Width = " << m_mapData->getCols()-2 << "\nHeight = "<< m_mapData->getRows()-2 << "\n";
    saveFile << "StartCell=";
    for(size_t i=0; i<startCells.size(); i++) {
        if (startCells[i].x != -1 && startCells[i].y != -1) {
            if (i !=0) {
                saveFile << ",";
            }
            saveFile << startCells[i].y * m_mapData->getCols() + startCells[i].x ;
        }
    }
    saveFile << "\n\n\n[MAP]\n";
    // map data
    for(size_t j=1; j<m_mapData->getRows()-1; j++) {
        for(size_t i=1; i<m_mapData->getCols()-1; i++) {
            switch ((*m_mapData)[j][i]) {
                case TERRAIN_SAND:
                    saveFile << ')';
                    break;
                case TERRAIN_ROCK:
                    saveFile << '^';
                    break;
                case TERRAIN_MOUNTAIN:
                    saveFile << 'R';
                    break;
                case TERRAIN_SPICEHILL:
                    saveFile << '+';
                    break;
                case TERRAIN_SPICE:
                    saveFile << '-';
                    break;
                case TERRAIN_HILL:
                    saveFile << 'h';
                    break;
                default:
                    std::cout << "Unknown value " << ((*m_mapData)[j][i]) << std::endl;
                    break;
            }
        }
        saveFile << "\n";
    }

    saveFile.close();
}