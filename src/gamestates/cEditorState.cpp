#include "gamestates/cEditorState.h"
#include "controls/eKeyAction.h"
#include "gui/GuiBar.h"
#include "gui/GuiStateButton.h"
#include "gui/GuiButtonGroup.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "game/cGameInterface.h"
#include "map/cPreviewMaps.h"
#include "data/gfxeditor.h"

#include <cassert>
#include <iostream>
#include <fstream>

const int heightBarSize = 48;
const int heightButtonSize = 40;
const int minTileSize = 4;
const int maxTileSize = 64;
const int deltaTileSize = 4;
const Color editorGridColor = Color{128, 128, 128, 180};
const Color editorCenterLineColor = Color{32, 176, 32, 220};


cEditorState::cEditorState(sGameServices* services) 
    : cGameState(services),
    m_gfxdata(m_ctx->getGraphicsContext()->gfxdata.get()),
    m_gfxeditor(m_ctx->getGraphicsContext()->gfxeditor.get()),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface())
{
    assert(m_gfxdata != nullptr);
    assert(m_gfxeditor != nullptr);
    assert(m_settings != nullptr);
    assert(m_interface != nullptr);

    const cRectangle &selectRect = cRectangle(0, 0, m_settings->getScreenW(), heightBarSize);
    const cRectangle &modifRect = cRectangle(m_settings->getScreenW()-heightBarSize, heightBarSize, heightBarSize, m_settings->getScreenH()-heightBarSize);
    mapSizeArea = cRectangle(0,heightBarSize,m_settings->getScreenW()-heightBarSize,m_settings->getScreenH()-heightBarSize);
    m_selectBar = std::make_unique<GuiBar>(m_renderDrawer, selectRect,GuiBarPlacement::HORIZONTAL,heightButtonSize);
    m_topologyBar = std::make_unique<GuiBar>(m_renderDrawer, modifRect,GuiBarPlacement::VERTICAL, heightButtonSize);
    m_startCellBar = std::make_unique<GuiBar>(m_renderDrawer, modifRect,GuiBarPlacement::VERTICAL, heightButtonSize);
    m_symmetricBar = std::make_unique<GuiBar>(m_renderDrawer, modifRect,GuiBarPlacement::VERTICAL, heightButtonSize);

    m_selectBar->setTheme(cGuiThemeBuilder().light().build());
    m_selectBar->beginPlacement(heightBarSize);
    m_topologyBar->setTheme(cGuiThemeBuilder().light().build());
    m_topologyBar->beginPlacement(heightBarSize);
    m_startCellBar->setTheme(cGuiThemeBuilder().light().build());
    m_startCellBar->beginPlacement(heightBarSize);
    m_symmetricBar->setTheme(cGuiThemeBuilder().light().build());
    m_symmetricBar->beginPlacement(heightBarSize);

    populateSelectBar();
    populateTopologyBar();
    populateStartCellBar();
    populateSymmetricBar();
    startCells.fill({-1, -1});
}

cEditorState::~cEditorState()
{

}

void cEditorState::populateSelectBar()
{
    m_selectGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(0, 0, heightButtonSize, heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STONELAYER))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                m_currentBar = m_topologyBar.get();
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    guiButton->setPressed(true);
    m_selectBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                m_currentBar = m_startCellBar.get();
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(FLIPHORZ))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                m_currentBar = m_symmetricBar.get();
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addAutoGuiObject(std::move(guiButton));
}

void cEditorState::populateTopologyBar()
{
    m_topologyGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(0,0,heightButtonSize,heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_HILL))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idTerrainToMapModif = TERRAIN_HILL;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_MOUNTAIN))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idTerrainToMapModif = TERRAIN_MOUNTAIN;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_ROCK))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idTerrainToMapModif = TERRAIN_ROCK;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SAND))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idTerrainToMapModif = TERRAIN_SAND;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    guiButton->setPressed(true);
    m_topologyBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SPICE))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idTerrainToMapModif = TERRAIN_SPICE;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(TERRAN_SPICEHILL))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idTerrainToMapModif = TERRAIN_SPICEHILL;
            })
            .build();
    guiButton->setGroup(m_topologyGroup.get());
    m_topologyBar->addAutoGuiObject(std::move(guiButton));
}

void cEditorState::populateStartCellBar()
{
    m_startCellGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(0, 0, heightButtonSize, heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION1))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idStartCellPlayer = 0;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    guiButton->setPressed(true);
    m_startCellBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION2))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idStartCellPlayer = 1;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION3))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idStartCellPlayer = 2;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION4))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idStartCellPlayer = 3;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION5))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                idStartCellPlayer = 4;
            })
            .build();
    guiButton->setGroup(m_startCellGroup.get());
    m_startCellBar->addAutoGuiObject(std::move(guiButton));
}

void cEditorState::populateSymmetricBar()
{
    m_symmetricGroup = std::make_unique<GuiButtonGroup>();
    auto rectGui = cRectangle(0, 0, heightButtonSize,heightButtonSize);
    auto guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(FLIPHORZLR))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                // std::cout << "FLIPHORZLR" << std::endl;
                modifySymmetricArea(Direction::right);
            })
            .build();
    guiButton->setGroup(m_symmetricGroup.get());
    m_symmetricBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(FLIPHORZRL))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                // std::cout << "FLIPHORZRL" << std::endl;
                modifySymmetricArea(Direction::left);
            })
            .build();
    guiButton->setGroup(m_symmetricGroup.get());
    m_symmetricBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(FLIPVERTBT))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                // std::cout << "FLIPVERTBT" << std::endl;
                modifySymmetricArea(Direction::top);
            })
            .build();
    guiButton->setGroup(m_symmetricGroup.get());
    m_symmetricBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(FLIPVERTTB))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                // std::cout << "FLIPVERTTB" << std::endl;
                modifySymmetricArea(Direction::bottom);
            })
            .build();
    guiButton->setGroup(m_symmetricGroup.get());
    m_symmetricBar->addAutoGuiObject(std::move(guiButton));
}

void cEditorState::thinkFast()
{
}

void cEditorState::draw() const
{
    drawMap();
    if (m_displayGrid)
        drawGrid();
    if (m_displayAxes)
        drawAxes();
    if (m_currentBar == m_startCellBar.get())
        drawStartCells();
    m_selectBar->draw();
    m_currentBar->draw();
    m_interface->drawCursor();
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
            updateVisibleTiles();
        } else if (event.eventType == MOUSE_SCROLLED_UP) {
            zoomAtMapPosition(mouseX, mouseY, ZoomDirection::zoomIn);
            updateVisibleTiles();
        } else if (event.eventType == MOUSE_LEFT_BUTTON_PRESSED && m_currentBar == m_topologyBar.get()) {
            modifyTile(mouseX, mouseY, idTerrainToMapModif);
        }else if (event.eventType == MOUSE_LEFT_BUTTON_PRESSED && m_currentBar == m_startCellBar.get()) {
            modifyStartCell(mouseX, mouseY, idStartCellPlayer);
        }        
        else {
            return;
        }
	// to test : updateVisibleTiles();
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
        if (event.isType(eKeyEventType::PRESSED) && event.isAction(eKeyAction::MENU_BACK)) {
            m_interface->setTransitionToWithFadingOut(GAME_MENU);
        }
        return;
    }
    if (event.isType(eKeyEventType::PRESSED)) {
        if (event.isAction(eKeyAction::MENU_BACK)) {
            m_interface->setTransitionToWithFadingOut(GAME_MENU);
        }
        if (event.isAction(eKeyAction::EDITOR_SAVE)) {
            //std::cout << "Save" << std::endl;
            saveMap();
        }
        if (event.isAction(eKeyAction::EDITOR_ZOOM_IN)) {
            zoomAtMapPosition(m_settings->getScreenW()/2, m_settings->getScreenH()/2, ZoomDirection::zoomIn);
            updateVisibleTiles();
        }
        if (event.isAction(eKeyAction::EDITOR_ZOOM_OUT)) {
            zoomAtMapPosition(m_settings->getScreenW()/2, m_settings->getScreenH()/2, ZoomDirection::zoomOut);
            updateVisibleTiles();
        }
        // to test : updateVisibleTiles();
        m_selectBar->onNotifyKeyboardEvent(event);
        m_currentBar->onNotifyKeyboardEvent(event);

        if (event.isAction(eKeyAction::EDITOR_DISPLAY_GRID)) {
            // toggle grid display
            //std::cout << "Toggle grid display" << std::endl;
            m_displayGrid = !m_displayGrid;

        }
        if (event.isAction(eKeyAction::EDITOR_DISPLAY_AXES)) {
            // toggle axes display
            //std::cout << "Toggle axes display" << std::endl;
            m_displayAxes = !m_displayAxes;
        }
    }

    if (event.isType(eKeyEventType::HOLD)) {
        if (event.isAction(eKeyAction::SCROLL_LEFT)) {
            cameraX -=tileLenSize;
            clampCameraXToMapBounds();
            updateVisibleTiles();
        }
        if (event.isAction(eKeyAction::SCROLL_RIGHT)) {
            cameraX +=tileLenSize;
            clampCameraXToMapBounds();
            updateVisibleTiles();
        }
        if (event.isAction(eKeyAction::SCROLL_UP)) {
            cameraY -=tileLenSize;
            clampCameraYToMapBounds();
            updateVisibleTiles();
        }
        if (event.isAction(eKeyAction::SCROLL_DOWN)) {
            cameraY +=tileLenSize;
            clampCameraYToMapBounds();
            updateVisibleTiles();
        }
        if (event.isShiftPressed() && event.isAction(eKeyAction::SCROLL_UP)) {
            zoomAtMapPosition(m_settings->getScreenW()/2, m_settings->getScreenH()/2, ZoomDirection::zoomIn);
            updateVisibleTiles();
        }
        if (event.isShiftPressed() && event.isAction(eKeyAction::SCROLL_DOWN)) {
            zoomAtMapPosition(m_settings->getScreenW()/2, m_settings->getScreenH()/2, ZoomDirection::zoomOut);
            updateVisibleTiles();
        }
        //to test : updateVisibleTiles();
    }
}

void cEditorState::loadMap(s_PreviewMap* map)
{
    //std::cout << "open |"<< map->name << "|" << std::endl;
    m_mapData = std::make_unique<Matrix<int>>(map->terrainType, map->height, map->width);
    m_mapAuthor = map->author;
    m_mapDescription = map->description;
    m_mapName = map->name;
    for (int i=0; i<MAX_SKIRMISHMAP_PLAYERS; i++) {
        if (map->iStartCell[i] !=-1) {
            //std::cout << "startCell " << map->iStartCell[i] << std::endl;
            int w = map->iStartCell[i] % map->width;
            int h = map->iStartCell[i] / map->width;
            startCells[i] = cPoint(w,h);
        }
    }
    updateVisibleTiles();
    normalizeModifications();
    m_displayGrid = false;
    m_displayAxes = false;
}

void cEditorState::normalizeModifications()
{
    for (size_t j = 0; j < m_mapData->getRows(); j++) {
        (*m_mapData)[j][0] = -1;
        (*m_mapData)[j][m_mapData->getCols()-1] = -1;
    }
    for (size_t i = 0; i < m_mapData->getCols(); i++) {
        (*m_mapData)[0][i] = -1;
        (*m_mapData)[m_mapData->getRows()-1][i] = -1;
    }
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
    int maxCameraY = m_mapData->getRows() * tileLenSize - mapSizeArea.getHeight();
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
    int maxCameraX = m_mapData->getCols() * tileLenSize - mapSizeArea.getWidth();
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
                m_renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SPICE), srcRect, destRect);
                break;
            case TERRAIN_SAND:
                m_renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SAND), srcRect, destRect);
                break;
            case TERRAIN_MOUNTAIN:
                m_renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_MOUNTAIN), srcRect, destRect);
                break;
            case TERRAIN_ROCK:
                m_renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_ROCK), srcRect, destRect);
                break;
            case TERRAIN_SPICEHILL:
                m_renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_SPICEHILL), srcRect, destRect);
                break;  
            case TERRAIN_HILL:
                m_renderDrawer->renderStrechSprite(m_gfxdata->getTexture(TERRAIN_HILL), srcRect, destRect);
                break;            
            default:
                break;
            }
        }
    }
}

void cEditorState::drawGrid() const
{
    if (m_mapData == nullptr) {
        return;
    }

    const int viewportTop = heightBarSize;
    const int viewportBottom = heightBarSize + mapSizeArea.getHeight() - 1;
    const int viewportLeft = 0;
    const int viewportRight = mapSizeArea.getWidth() - 1;

    const int mapLeftOnScreen = -cameraX;
    const int mapRightOnScreen = static_cast<int>(m_mapData->getCols() * tileLenSize) - cameraX;
    const int mapTopOnScreen = heightBarSize - cameraY;
    const int mapBottomOnScreen = heightBarSize + static_cast<int>(m_mapData->getRows() * tileLenSize) - cameraY;

    const int gridLeft = std::max(viewportLeft, mapLeftOnScreen);
    const int gridRight = std::min(viewportRight, mapRightOnScreen);
    const int gridTop = std::max(viewportTop, mapTopOnScreen);
    const int gridBottom = std::min(viewportBottom, mapBottomOnScreen);

    if (gridLeft > gridRight || gridTop > gridBottom) {
        return;
    }

    for (int i = startX; i <= static_cast<int>(endX); i++) {
        int lineX = (i * tileLenSize) - cameraX;
        if (lineX < gridLeft || lineX > gridRight) {
            continue;
        }
        m_renderDrawer->renderLine(lineX, gridTop, lineX, gridBottom, editorGridColor);
    }

    for (int j = startY; j <= static_cast<int>(endY); j++) {
        int lineY = heightBarSize + (j * tileLenSize) - cameraY;
        if (lineY < gridTop || lineY > gridBottom) {
            continue;
        }
        m_renderDrawer->renderLine(gridLeft, lineY, gridRight, lineY, editorGridColor);
    }
}

void cEditorState::drawAxes() const
{
    if (m_mapData == nullptr) {
        return;
    }

    const int viewportTop = heightBarSize;
    const int viewportBottom = heightBarSize + mapSizeArea.getHeight() - 1;
    const int viewportLeft = 0;
    
    const int mapLeftOnScreen = -cameraX;
    const int mapRightOnScreen = static_cast<int>(m_mapData->getCols() * tileLenSize) - cameraX;
    const int mapTopOnScreen = heightBarSize - cameraY;
    const int mapBottomOnScreen = heightBarSize + static_cast<int>(m_mapData->getRows() * tileLenSize) - cameraY;

    const int viewportRight = mapSizeArea.getWidth() - 1;
    const int gridLeft = std::max(viewportLeft, mapLeftOnScreen);
    const int gridRight = std::min(viewportRight, mapRightOnScreen);
    const int gridTop = std::max(viewportTop, mapTopOnScreen);
    const int gridBottom = std::min(viewportBottom, mapBottomOnScreen);

    if (gridLeft > gridRight || gridTop > gridBottom) {
        return;
    }

    const int middleX = (static_cast<int>(m_mapData->getCols()) * tileLenSize) / 2 - cameraX;
    if (middleX >= gridLeft && middleX <= gridRight) {
        m_renderDrawer->renderLine(middleX, gridTop, middleX, gridBottom, editorCenterLineColor);
    }

    const int middleY = heightBarSize + (static_cast<int>(m_mapData->getRows()) * tileLenSize) / 2 - cameraY;
    if (middleY >= gridTop && middleY <= gridBottom) {
        m_renderDrawer->renderLine(gridLeft, middleY, gridRight, middleY, editorCenterLineColor);
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
    if (m_mapData && tileX >= 1 && tileY >= 1 && tileX < (int)m_mapData->getCols()-1 && tileY < (int)m_mapData->getRows()-1) {
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
    if (m_mapData && tileX >= 1 && tileY >= 1 && tileX < (int)m_mapData->getCols()-1 && tileY < (int)m_mapData->getRows()-1) {
        startCells[startCellID]={tileX, tileY};
    }
}

void cEditorState::modifySymmetricArea(Direction dir)
{
    // std::cout << "modifySymmetricArea " << static_cast<int>(dir) << std::endl;
    switch (dir) {
        case Direction::bottom:
            for (size_t j = 1; j < (m_mapData->getRows())/2; j++) {
                for (size_t i = 1; i < m_mapData->getCols(); i++) {
                    (*m_mapData)[(m_mapData->getRows()-1)-j][i] = (*m_mapData)[j][i];
                }
            }
            break;
        case Direction::top:
            for (size_t j = 1; j < (m_mapData->getRows())/2; j++) {
                for (size_t i = 1; i < m_mapData->getCols(); i++) {
                    (*m_mapData)[j][i] = (*m_mapData)[(m_mapData->getRows()-1)-j][i];
                }
            }
            break;
        case Direction::right:
            for (size_t j = 1; j < m_mapData->getRows(); j++) {
                for (size_t i = 1; i < (m_mapData->getCols())/2; i++) {
                    (*m_mapData)[j][(m_mapData->getCols()-1)-i] = (*m_mapData)[j][i];
                }
            }
            break;
        case Direction::left:
            for (size_t j = 1; j < m_mapData->getRows(); j++) {
                for (size_t i = 1; i < (m_mapData->getCols())/2; i++) {
                    (*m_mapData)[j][i] = (*m_mapData)[j][(m_mapData->getCols()-1)-i];
                }
            }
            break;
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
            if (x + tileLenSize > 0 && x < m_settings->getScreenW() &&y + tileLenSize > heightBarSize && y < m_settings->getScreenH()) {
                destRect = cRectangle(x, y, tileLenSize, tileLenSize);
                m_renderDrawer->renderStrechSprite(m_gfxeditor->getTexture(STARTPOSITION1+i), srcRect, destRect);
            }
        }
    }
}

static std::string normalizeStringForFileName(const std::string& input)
{
    std::string output = input;
    // Replace spaces with underscores
    std::replace(output.begin(), output.end(), ' ', '_');
    // Remove characters that are not alphanumeric or underscores
    output.erase(std::remove_if(output.begin(), output.end(),
        [](unsigned char c) { return !std::isalnum(c) && c != '_'; }), output.end());
    if (output.empty()) {
        output = "custom_map";
        //@mira : log warning about empty name after normalization*/
    }
    return output;
}

void cEditorState::saveMap() const
{
    // creating file
    std::string fileName = "skirmish/"+normalizeStringForFileName(m_mapName)+".ini";
    std::ofstream saveFile(fileName);
    // file verification
    if (!saveFile.is_open()){
        std::cerr << "unable to open modified map for saving " << fileName << std::endl;
        //@mira : log error about unable to save map*/
        return;
    }
    // map card
    saveFile << "[SKIRMISH]\n";
    saveFile << "Title = " << m_mapName << "\n";
    saveFile << "Author = "<< m_mapAuthor << "\n";
    saveFile << "Description = "<< m_mapDescription << "\n\n";
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
    //@mira : log info about successful map saving*/
}
