#include "gamestates/cEditorState.h"
#include "gamestates/cEditorState/cEditorCam.h"
#include "gamestates/cEditorState/cEditorUndoRedoHistory.h"
#include "controls/eKeyAction.h"
#include "gui/GuiBar.h"
#include "gui/GuiStateButton.h"
#include "gui/GuiButton.h"
#include "gui/GuiButtonGroup.h"
#include "gui/GuiValueButton.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "utils/Graphics.hpp"
#include "context/GameContext.hpp"
#include "context/cGameObjectContext.h"
#include "game/cGameInterface.h"
#include "controls/cMouse.h"
#include "gameobjects/map/cPreviewMaps.h"
#include "data/gfxeditor.h"
#include "include/sGameEvent.h"

#include "include/cAssert.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <memory>

const int heightBarSize = 48;
const int heightButtonSize = 40;
const int maxPenSize = 11; // must be odd to have a center
const Color editorGridColor = Color{128, 128, 128, 180};
const Color editorCenterLineColor = Color{32, 176, 32, 220};
const Color editorHoveredCellFillColor = Color{255, 230, 64, 96};
const Color editorHoveredCellBorderColor = Color{255, 230, 64, 220};
const Color editorSelectionFillColor = Color{255, 255, 255, 40};
const Color editorSelectionBorderColor = Color{255, 255, 255, 255};
const Color editorPasteGhostFillColor = Color{64, 208, 255, 60};
const Color editorPasteGhostBorderColor = Color{64, 208, 255, 220};


cEditorState::cEditorState(sGameServices* services) 
    : cGameState(services),
    m_gfxdata(m_ctx->getGraphicsContext()->gfxdata.get()),
    m_gfxeditor(m_ctx->getGraphicsContext()->gfxeditor.get()),
    m_settings(services->settings),
    m_interface(m_ctx->getGameInterface()),
    m_textDrawer(m_ctx->getTextContext()->getBeneTextDrawer()),
    m_previewMaps(services->objects->getPreviewMaps())
{
    d2tm_assert(m_gfxdata != nullptr);
    d2tm_assert(m_gfxeditor != nullptr);
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_interface != nullptr);
    d2tm_assert(m_textDrawer != nullptr);

    m_undoRedo = std::make_unique<cEditorUndoRedoHistory>();

    const cRectangle &selectRect = cRectangle(0, 0, m_settings->getScreenW(), heightBarSize);
    const cRectangle &modifRect = cRectangle(m_settings->getScreenW()-heightBarSize, heightBarSize, heightBarSize, m_settings->getScreenH()-heightBarSize);
    mapSizeArea = cRectangle(0,heightBarSize,m_settings->getScreenW()-heightBarSize,m_settings->getScreenH()-heightBarSize);
    m_editorCam = std::make_unique<cEditorCam>(mapSizeArea);
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
    setCurrentBar(m_topologyBar.get());
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
                setCurrentBar(m_topologyBar.get());
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectGroup->updateStates(guiButton.get());
    m_selectBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(STARTPOSITION))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                setCurrentBar(m_startCellBar.get());
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addAutoGuiObject(std::move(guiButton));

    guiButton = GuiStateButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(FLIPHORZ))
            .withRenderer(m_renderDrawer)
            .onClick([this]() {
                setCurrentBar(m_symmetricBar.get());
            })
            .build();
    guiButton->setGroup(m_selectGroup.get());
    m_selectBar->addAutoGuiObject(std::move(guiButton));

    rectGui = cRectangle(m_settings->getScreenW()-heightButtonSize*3, (heightBarSize-heightButtonSize)/2, heightButtonSize, heightButtonSize);
    // std::cout << "Save icon position: x=" << rectGui.getX() << " y=" << rectGui.getY() << " w= " << rectGui.getWidth() << " h= " << rectGui.getHeight() << std::endl;
    auto guiIcon = GuiButtonBuilder()
            .withRect(rectGui)
            .withTexture(m_gfxeditor->getTexture(SAVEICON))
            .withRenderer(m_renderDrawer)
            .withKind(GuiRenderKind::WITH_STRETCHED_TEXTURE)
            .onClick([this]() {
                saveMap();
                m_hasChanged = false;
            })
            .build();
    m_selectBar->addGuiObject(std::move(guiIcon));
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
    m_topologyGroup->updateStates(guiButton.get());
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

    auto cursorSizeButton = GuiValueButtonBuilder()
            .withRect(cRectangle(0, 0, heightButtonSize, heightButtonSize))
            .withRenderer(m_renderDrawer)
            .withTextDrawer(m_textDrawer)
            .withTheme(cGuiThemeBuilder().light().build())
            .withLabel("size")
            .withInitialValue(m_topologyCursorSize)
            .withStepValue(2)
            .withMinValue(1)
            .withMaxValue(maxPenSize)
            .onChanged([this](int value) {
                setCursorSize(value);
            })
            .build();
    m_topologyBar->addAutoGuiObject(std::move(cursorSizeButton));
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
    m_startCellGroup->updateStates(guiButton.get());
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
    drawHoveredCellHighlight();
    drawSelectionRectangle();
    if (m_displaySelection)
        drawPastePreviewGhost();
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

void cEditorState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (m_mapData == nullptr) {
        return;
    }

    if (event.eventType == MOUSE_RIGHT_BUTTON_CLICKED) {
        if (m_hasSelection) {
            const int selectionWidth = m_selectionEndTileX - m_selectionStartTileX + 1;
            const int selectionHeight = m_selectionEndTileY - m_selectionStartTileY + 1;
            if (selectionWidth * selectionHeight < 2) {
                m_hasSelection = false;
            }
        }
        m_isDraggingSelection = false;
    }

    if (event.coords.isWithinRectangle(&mapSizeArea)) {
        // Zoom centered on the cursor
        int mouseX = event.coords.x;
        int mouseY = event.coords.y - mapSizeArea.getY(); // offset barre
        if (event.eventType == MOUSE_SCROLLED_DOWN) {
            m_editorCam->zoomAtMapPosition(mouseX, mouseY, cEditorCam::ZoomDirection::zoomOut, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        } else if (event.eventType == MOUSE_SCROLLED_UP) {
            m_editorCam->zoomAtMapPosition(mouseX, mouseY, cEditorCam::ZoomDirection::zoomIn, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        } else if (event.eventType == MOUSE_RIGHT_BUTTON_PRESSED) {
            int tileX = 0;
            int tileY = 0;
            if (tryGetTileFromMouseCoords(event.coords, tileX, tileY) && isEditableTile(tileX, tileY)) {
                if (!m_isDraggingSelection) {
                    m_selectionAnchorTileX = tileX;
                    m_selectionAnchorTileY = tileY;
                    m_isDraggingSelection = true;
                    m_hasSelection = true;
                }
                updateSelectionFromTiles(m_selectionAnchorTileX, m_selectionAnchorTileY, tileX, tileY);
            }
        } else if (event.eventType == MOUSE_MOVED_TO && m_isDraggingSelection) {
            int tileX = 0;
            int tileY = 0;
            if (tryGetTileFromMouseCoords(event.coords, tileX, tileY)) {
                updateSelectionFromTiles(m_selectionAnchorTileX, m_selectionAnchorTileY, tileX, tileY);
            }
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
            if (m_hasChanged) {
                saveMap(true);
                m_hasChanged = false;
            }
            m_interface->setNextStateToTransitionTo(GAME_OPTIONS);
        }
        if (event.isAction(eKeyAction::EDITOR_SAVE)) {
            //std::cout << "Save" << std::endl;
            saveMap();
            m_hasChanged = false;
        }
        if (event.isAction(eKeyAction::EDITOR_ZOOM_IN)) {
            m_editorCam->zoomAtMapPosition(mapSizeArea.getWidth()/2, mapSizeArea.getHeight()/2, cEditorCam::ZoomDirection::zoomIn, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        }
        if (event.isAction(eKeyAction::EDITOR_ZOOM_OUT)) {
            m_editorCam->zoomAtMapPosition(mapSizeArea.getWidth()/2, mapSizeArea.getHeight()/2, cEditorCam::ZoomDirection::zoomOut, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        }
        // to test : updateVisibleTiles();
        //m_selectBar->onNotifyKeyboardEvent(event);
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
        if (event.isAction(eKeyAction::EDITOR_UNDO)) {
            if (m_undoRedo->applyUndo(*m_mapData, startCells)) {
                m_hasChanged = true;
            }
        }
        if (event.isAction(eKeyAction::EDITOR_REDO)) {
            if (m_undoRedo->applyRedo(*m_mapData, startCells)) {
                m_hasChanged = true;
            }
        }
        if (event.isAction(eKeyAction::EDITOR_COPY)) {
            std::cout << "Action : Copy selection to clipboard" << std::endl;
            copySelectionToClipboard();
        }
        if (event.isAction(eKeyAction::EDITOR_PASTE)) {
            std::cout << "Action : Paste selection to clipboard" << std::endl;
            pasteClipboardAtMouseCursor();
        }
        if (!event.isShiftPressed()) {
            m_displaySelection = false;
        }
    }

    if (event.isType(eKeyEventType::HOLD)) {
        if (event.isAction(eKeyAction::SCROLL_LEFT)) {
            m_editorCam->scrollByTiles(-1, 0, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        }
        if (event.isAction(eKeyAction::SCROLL_RIGHT)) {
            m_editorCam->scrollByTiles(1, 0, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        }
        if (event.isAction(eKeyAction::SCROLL_UP)) {
            m_editorCam->scrollByTiles(0, -1, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        }
        if (event.isAction(eKeyAction::SCROLL_DOWN)) {
            m_editorCam->scrollByTiles(0, 1, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
        }
        if (event.isShiftPressed() && event.isAction(eKeyAction::SCROLL_UP)) {
            m_editorCam->zoomAtMapPosition(mapSizeArea.getWidth()/2, mapSizeArea.getHeight()/2, cEditorCam::ZoomDirection::zoomIn, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
            return;
        }
        if (event.isShiftPressed() && event.isAction(eKeyAction::SCROLL_DOWN)) {
            m_editorCam->zoomAtMapPosition(mapSizeArea.getWidth()/2, mapSizeArea.getHeight()/2, cEditorCam::ZoomDirection::zoomOut, *m_mapData);
            m_editorCam->updateVisibleTiles(*m_mapData);
            return;
        }
        if (event.isShiftPressed()) {
            m_displaySelection = true;
        }
        //to test : updateVisibleTiles();
    }
}

void cEditorState::loadMap(int mapIndex)
{
    s_PreviewMap* map = m_previewMaps->getMap(mapIndex);
    //std::cout << "open |"<< map->name << "|" << std::endl;
    m_mapData = std::make_unique<Matrix<int>>(map->terrainType, map->height, map->width);
    m_mapAuthor = map->author;
    m_mapDescription = map->description;
    m_mapName = map->name;
    startCells.fill({-1, -1});
    for (int i=0; i<MAX_SKIRMISHMAP_PLAYERS; i++) {
        if (map->iStartCell[i] !=-1) {
            //std::cout << "startCell " << map->iStartCell[i] << std::endl;
            int w = map->iStartCell[i] % map->width;
            int h = map->iStartCell[i] / map->width;
            startCells[i] = cPoint(w,h);
        }
    }
    m_editorCam->reset();
    m_editorCam->updateVisibleTiles(*m_mapData);
    normalizeModifications();
    m_displayGrid = false;
    m_displayAxes = false;
    m_displaySelection = false;
    m_hasChanged = false;
    m_hasSelection = false;
    m_isDraggingSelection = false;
    m_undoRedo->clear();
}

void cEditorState::setCursorSize(int value)
{
    int normalizedValue = std::max(1, value);
    if (normalizedValue % 2 == 0) {
        normalizedValue += 1;
    }

    normalizedValue = std::min(normalizedValue, maxPenSize);
    if (normalizedValue % 2 == 0) {
        normalizedValue -= 1;
    }

    m_topologyCursorSize = normalizedValue;
    m_cursorSize = getActiveCursorSize();
}

void cEditorState::setCurrentBar(GuiBar* bar)
{
    m_currentBar = bar;
    m_cursorSize = getActiveCursorSize();
}

int cEditorState::getActiveCursorSize() const
{
    if (m_currentBar == m_topologyBar.get()) {
        return m_topologyCursorSize;
    }

    return 1;
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

void cEditorState::drawMap() const
{
    if (m_mapData == nullptr) {
        return;
    }

    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();
    const int startX = m_editorCam->getStartX();
    const int startY = m_editorCam->getStartY();
    const size_t endX = m_editorCam->getEndX();
    const size_t endY = m_editorCam->getEndY();
    
    int tileID;
    int tile_world_x, tile_world_y;
    int tile_screen_x, tile_screen_y;

    cRectangle destRect;
    cRectangle srcRect{0,0,32,32}; // we take the first full textured sprite
    for (size_t j = static_cast<size_t>(startY); j < endY; j++) {
        for (size_t i = static_cast<size_t>(startX); i < endX; i++) {
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

void cEditorState::drawHoveredCellHighlight() const
{
    if (m_mapData == nullptr) {
        return;
    }

    cMouse *mouse = m_interface->getMouse();
    if (mouse == nullptr) {
        return;
    }

    const cPoint mouseCoords = mouse->getMouseCoords();
    if (!mouseCoords.isWithinRectangle(&mapSizeArea)) {
        return;
    }

    const int mouseMapX = mouseCoords.x;
    const int mouseMapY = mouseCoords.y - mapSizeArea.getY();
    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();

    const int tileX = (cameraX + mouseMapX) / tileLenSize;
    const int tileY = (cameraY + mouseMapY) / tileLenSize;

    if (tileX < 0 || tileY < 0 || tileX >= static_cast<int>(m_mapData->getCols()) || tileY >= static_cast<int>(m_mapData->getRows())) {
        return;
    }

    int highlightStartTileX = 0;
    int highlightStartTileY = 0;
    int highlightEndTileX = 0;
    int highlightEndTileY = 0;
    getBrushTileBounds(tileX, tileY, highlightStartTileX, highlightStartTileY, highlightEndTileX, highlightEndTileY);

    if (highlightStartTileX > highlightEndTileX || highlightStartTileY > highlightEndTileY) {
        return;
    }

    const int cellScreenX = highlightStartTileX * tileLenSize - cameraX;
    const int cellScreenY = heightBarSize + highlightStartTileY * tileLenSize - cameraY;
    const int highlightWidth = (highlightEndTileX - highlightStartTileX + 1) * tileLenSize;
    const int highlightHeight = (highlightEndTileY - highlightStartTileY + 1) * tileLenSize;

    cRectangle cellRect(cellScreenX, cellScreenY, highlightWidth, highlightHeight);
    m_renderDrawer->renderRectFillColor(cellRect, editorHoveredCellFillColor, editorHoveredCellFillColor.a);
    m_renderDrawer->renderRectColor(cellRect, editorHoveredCellBorderColor, editorHoveredCellBorderColor.a);
}

void cEditorState::drawSelectionRectangle() const
{
    if (m_mapData == nullptr || !m_hasSelection) {
        return;
    }
    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();

    const int cellScreenX = m_selectionStartTileX * tileLenSize - cameraX;
    const int cellScreenY = heightBarSize + m_selectionStartTileY * tileLenSize - cameraY;
    const int highlightWidth = (m_selectionEndTileX - m_selectionStartTileX + 1) * tileLenSize;
    const int highlightHeight = (m_selectionEndTileY - m_selectionStartTileY + 1) * tileLenSize;

    cRectangle selectionRect(cellScreenX, cellScreenY, highlightWidth, highlightHeight);
    m_renderDrawer->renderRectFillColor(selectionRect, editorSelectionFillColor, editorSelectionFillColor.a);
    m_renderDrawer->renderRectColor(selectionRect, editorSelectionBorderColor, editorSelectionBorderColor.a);
}

void cEditorState::drawPastePreviewGhost() const
{
    if (m_mapData == nullptr || m_clipboardTiles.empty() || m_clipboardTiles.front().empty()) {
        return;
    }

    cMouse *mouse = m_interface->getMouse();
    int anchorTileX = 0;
    int anchorTileY = 0;
    if (!tryGetTileFromMouseCoords(mouse->getMouseCoords(), anchorTileX, anchorTileY)) {
        return;
    }

    const int widthInTiles = static_cast<int>(m_clipboardTiles.front().size());
    const int heightInTiles = static_cast<int>(m_clipboardTiles.size());
    if (widthInTiles <= 0 || heightInTiles <= 0) {
        return;
    }

    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();

    const int cellScreenX = anchorTileX * tileLenSize - cameraX;
    const int cellScreenY = heightBarSize + anchorTileY * tileLenSize - cameraY;
    const int ghostWidth = widthInTiles * tileLenSize;
    const int ghostHeight = heightInTiles * tileLenSize;

    cRectangle ghostRect(cellScreenX, cellScreenY, ghostWidth, ghostHeight);
    m_renderDrawer->renderRectFillColor(ghostRect, editorPasteGhostFillColor, editorPasteGhostFillColor.a);
    m_renderDrawer->renderRectColor(ghostRect, editorPasteGhostBorderColor, editorPasteGhostBorderColor.a);
}

bool cEditorState::tryGetTileFromMouseCoords(const cPoint &coords, int &tileX, int &tileY) const
{
    if (!coords.isWithinRectangle(&mapSizeArea)) {
        return false;
    }

    tileX = m_editorCam->screenToTileX(coords.x);
    tileY = m_editorCam->screenToTileY(coords.y - mapSizeArea.getY());
    if (tileX < 0 || tileY < 0 || tileX >= static_cast<int>(m_mapData->getCols()) || tileY >= static_cast<int>(m_mapData->getRows())) {
        return false;
    }

    return true;
}

bool cEditorState::isEditableTile(int tileX, int tileY) const
{
    if (m_mapData == nullptr) {
        return false;
    }

    return tileX >= 1 && tileY >= 1 && tileX < static_cast<int>(m_mapData->getCols()) - 1 && tileY < static_cast<int>(m_mapData->getRows()) - 1;
}

void cEditorState::updateSelectionFromTiles(int startTileX, int startTileY, int endTileX, int endTileY)
{
    if (m_mapData == nullptr) {
        return;
    }

    const int minX = 1;
    const int minY = 1;
    const int maxX = static_cast<int>(m_mapData->getCols()) - 2;
    const int maxY = static_cast<int>(m_mapData->getRows()) - 2;

    startTileX = std::clamp(startTileX, minX, maxX);
    startTileY = std::clamp(startTileY, minY, maxY);
    endTileX = std::clamp(endTileX, minX, maxX);
    endTileY = std::clamp(endTileY, minY, maxY);

    m_selectionStartTileX = std::min(startTileX, endTileX);
    m_selectionStartTileY = std::min(startTileY, endTileY);
    m_selectionEndTileX = std::max(startTileX, endTileX);
    m_selectionEndTileY = std::max(startTileY, endTileY);
    m_hasSelection = true;
}

void cEditorState::copySelectionToClipboard()
{
    if (m_mapData == nullptr || !m_hasSelection) {
        return;
    }
    const int width = m_selectionEndTileX - m_selectionStartTileX + 1;
    const int height = m_selectionEndTileY - m_selectionStartTileY + 1;
    if (width <= 0 || height <= 0 || (width * height) < 2) {
        return;
    }

    m_clipboardTiles.assign(height, std::vector<int>(width, TERRAIN_SAND));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            m_clipboardTiles[y][x] = (*m_mapData)[m_selectionStartTileY + y][m_selectionStartTileX + x];
        }
    }
    m_hasSelection = false;
}

void cEditorState::pasteClipboardAtMouseCursor()
{
    if (m_mapData == nullptr || m_clipboardTiles.empty() || m_clipboardTiles.front().empty()) {
        return;
    }

    cMouse *mouse = m_interface->getMouse();
    int anchorTileX = 0;
    int anchorTileY = 0;
    if (!tryGetTileFromMouseCoords(mouse->getMouseCoords(), anchorTileX, anchorTileY)) {
        return;
    }

    bool hasRecordedGroup = false;
    const int height = static_cast<int>(m_clipboardTiles.size());
    const int width = static_cast<int>(m_clipboardTiles.front().size());
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const int destTileX = anchorTileX + x;
            const int destTileY = anchorTileY + y;
            if (!isEditableTile(destTileX, destTileY)) {
                continue;
            }

            const int newTileID = m_clipboardTiles[y][x];
            const int oldTileID = (*m_mapData)[destTileY][destTileX];
            if (oldTileID == newTileID) {
                continue;
            }

            if (!hasRecordedGroup) {
                m_undoRedo->beginRecordGroup();
                hasRecordedGroup = true;
            }

            m_undoRedo->recordTileChange(destTileX, destTileY, oldTileID, newTileID);
            (*m_mapData)[destTileY][destTileX] = newTileID;
        }
    }

    if (hasRecordedGroup) {
        m_hasChanged = true;
    }
}

int cEditorState::getNormalizedCursorSize() const
{
    int cursorSize = std::max(1, getActiveCursorSize());
    if (cursorSize % 2 == 0) {
        cursorSize += 1;
    }

    return cursorSize;
}

void cEditorState::getBrushTileBounds(int centerTileX, int centerTileY, int &brushStartTileX, int &brushStartTileY, int &brushEndTileX, int &brushEndTileY) const
{
    const int cursorRadius = getNormalizedCursorSize() / 2;

    brushStartTileX = std::max(1, centerTileX - cursorRadius);
    brushStartTileY = std::max(1, centerTileY - cursorRadius);
    brushEndTileX = std::min(static_cast<int>(m_mapData->getCols()) - 2, centerTileX + cursorRadius);
    brushEndTileY = std::min(static_cast<int>(m_mapData->getRows()) - 2, centerTileY + cursorRadius);
}

void cEditorState::drawGrid() const
{
    if (m_mapData == nullptr) {
        return;
    }

    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();
    const int startX = m_editorCam->getStartX();
    const int startY = m_editorCam->getStartY();
    const size_t endX = m_editorCam->getEndX();
    const size_t endY = m_editorCam->getEndY();

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

    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();

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

void cEditorState::modifyTile(int posX, int posY, int tileID)
{
    if (tileID == -1) {
        return;
    }

    const int tileX = m_editorCam->screenToTileX(posX);
    const int tileY = m_editorCam->screenToTileY(posY);
    if (m_mapData == nullptr || tileX < 1 || tileY < 1 || tileX >= static_cast<int>(m_mapData->getCols()) - 1 || tileY >= static_cast<int>(m_mapData->getRows()) - 1) {
        return;
    }

    int brushStartTileX = 0;
    int brushStartTileY = 0;
    int brushEndTileX = 0;
    int brushEndTileY = 0;
    getBrushTileBounds(tileX, tileY, brushStartTileX, brushStartTileY, brushEndTileX, brushEndTileY);

    bool hasRecordedGroup = false;
    for (int brushTileY = brushStartTileY; brushTileY <= brushEndTileY; brushTileY++) {
        for (int brushTileX = brushStartTileX; brushTileX <= brushEndTileX; brushTileX++) {
            int oldTileID = (*m_mapData)[brushTileY][brushTileX];
            if (oldTileID == tileID) {
                continue;
            }

            if (!hasRecordedGroup) {
                m_undoRedo->beginRecordGroup();
                hasRecordedGroup = true;
            }

            m_undoRedo->recordTileChange(brushTileX, brushTileY, oldTileID, tileID);
            (*m_mapData)[brushTileY][brushTileX] = tileID;
            m_hasChanged = true;
        }
    }
}

void cEditorState::modifyStartCell(int posX, int posY, int startCellID)
{
    if (startCellID == -1) {
        return;
    }
    const int tileX = m_editorCam->screenToTileX(posX);
    const int tileY = m_editorCam->screenToTileY(posY);
    if (m_mapData && tileX >= 1 && tileY >= 1 && tileX < (int)m_mapData->getCols()-1 && tileY < (int)m_mapData->getRows()-1) {
        cPoint oldPos = startCells[startCellID];
        cPoint newPos = {tileX, tileY};
        if (oldPos.x == newPos.x && oldPos.y == newPos.y) return;
        m_undoRedo->beginRecordGroup();
        m_undoRedo->recordStartCellChange(startCellID, oldPos, newPos);
        startCells[startCellID] = newPos;
        m_hasChanged = true;
    }
}

void cEditorState::modifySymmetricArea(Direction dir)
{
    //std::cout << "modifySymmetricArea " << static_cast<int>(dir) << std::endl;
    m_undoRedo->beginRecordGroup();
    switch (dir) {
        case Direction::bottom:
            for (size_t j = 1; j < (m_mapData->getRows())/2; j++) {
                for (size_t i = 1; i < m_mapData->getCols(); i++) {
                    size_t mirrorJ = (m_mapData->getRows()-1)-j;
                    int oldVal = (*m_mapData)[mirrorJ][i];
                    int newVal = (*m_mapData)[j][i];
                    if (oldVal != newVal) {
                        m_undoRedo->recordTileChange(i, mirrorJ, oldVal, newVal);
                        (*m_mapData)[mirrorJ][i] = newVal;
                    }
                }
            }
            break;
        case Direction::top:
            for (size_t j = 1; j < (m_mapData->getRows())/2; j++) {
                for (size_t i = 1; i < m_mapData->getCols(); i++) {
                    size_t mirrorJ = (m_mapData->getRows()-1)-j;
                    int oldVal = (*m_mapData)[j][i];
                    int newVal = (*m_mapData)[mirrorJ][i];
                    if (oldVal != newVal) {
                        m_undoRedo->recordTileChange(i, j, oldVal, newVal);
                        (*m_mapData)[j][i] = newVal;
                    }
                }
            }
            break;
        case Direction::right:
            for (size_t j = 1; j < m_mapData->getRows(); j++) {
                for (size_t i = 1; i < (m_mapData->getCols())/2; i++) {
                    size_t mirrorI = (m_mapData->getCols()-1)-i;
                    int oldVal = (*m_mapData)[j][mirrorI];
                    int newVal = (*m_mapData)[j][i];
                    if (oldVal != newVal) {
                        m_undoRedo->recordTileChange(mirrorI, j, oldVal, newVal);
                        (*m_mapData)[j][mirrorI] = newVal;
                    }
                }
            }
            break;
        case Direction::left:
            for (size_t j = 1; j < m_mapData->getRows(); j++) {
                for (size_t i = 1; i < (m_mapData->getCols())/2; i++) {
                    size_t mirrorI = (m_mapData->getCols()-1)-i;
                    int oldVal = (*m_mapData)[j][i];
                    int newVal = (*m_mapData)[j][mirrorI];
                    if (oldVal != newVal) {
                        m_undoRedo->recordTileChange(i, j, oldVal, newVal);
                        (*m_mapData)[j][i] = newVal;
                    }
                }
            }
            break;
    }
    m_hasChanged = true;
}

void cEditorState::drawStartCells() const
{
    cRectangle destRect;
    cRectangle srcRect{0,0,32,32}; // we take the first full textured sprite
    const int cameraX = m_editorCam->getCameraX();
    const int cameraY = m_editorCam->getCameraY();
    const int tileLenSize = m_editorCam->getTileSize();
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
        //TODO : log warning about empty name after normalization
    }
    return output;
}

void cEditorState::saveMap(bool backup) const
{
    std::string fileName;
    // creating file
    if (!backup) {
        // if no name provided, use map name
        fileName = "skirmish/"+normalizeStringForFileName(m_mapName)+".ini";
    } else {
        fileName = "skirmish/backup.ini";
    }
    
    std::ofstream saveFile(fileName);
    // file verification
    if (!saveFile.is_open()){
        std::cerr << "unable to open modified map for saving " << fileName << std::endl;
        //TODO : log error about unable to save map
        return;
    }
    //std::cout << "Saving map to " << fileName << std::endl;
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
    // TODO:  log info about successful map saving
    m_previewMaps->loadSkirmish(fileName);
    const s_GameEvent newEvent {
        .eventType = eGameEventType::GAME_EVENT_NOTIFICATION,
        .data = NotificationEvent {
            .message = "Map saved successfully",
            .type = eNotificationType::NEUTRAL,
        }
    };                
    m_interface->onNotifyGameEvent(newEvent);
}
