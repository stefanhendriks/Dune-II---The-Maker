/**
 * @file cEditorState.h
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
#include "cGameState.h"
#include "utils/cMatrix.h"
#include "controls/cKeyboardEvent.h"
#include "controls/sMouseEvent.h"
#include "utils/cRectangle.h"

#include <memory>
#include <array>
#include <string>
#include <vector>

class Texture;
class Graphics;

struct s_PreviewMap;
class cPreviewMaps;
class GuiBar;
class GuiButtonGroup;
class cTextDrawer;
class cMouse;
class cEditorUndoRedoHistory;
class cEditorCam;
class cEventEmitter;
class GameStateTransition;
class cGameSettings;

class cEditorState : public cGameState {
public:
    explicit cEditorState(sGameServices* services);
    ~cEditorState() override;

    void thinkFast() override;
    void draw() const override;
    // load mapIndex from cPreviewMaps, if -1 is given, load the currently edited map (m_editedMap)
    void loadMap(int mapIndex);
    void setCursorSize(int value);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
private:
    Graphics *m_gfxdata = nullptr;
    Graphics *m_gfxeditor = nullptr;
    cGameSettings *m_settings = nullptr;
    cEventEmitter* m_eventEmitter = nullptr;
    GameStateTransition* m_stateTransitions = nullptr;
    cMouse* m_mouse = nullptr;
    cTextDrawer *m_textDrawer = nullptr;
    cPreviewMaps* m_previewMaps = nullptr;
    std::string m_mapName, m_mapAuthor, m_mapDescription;
    enum class Direction : char { top, bottom, left, right, topLeft, topRight, bottomLeft, bottomRight };
    void populateTopologyBar();
    void populateStartCellBar();
    void populateSelectBar();
    void populateSymmetricBar();
    void setCurrentBar(GuiBar* bar);
    void modifySymmetricArea(Direction dir);
    void mirrorMarkerCells(Direction dir, std::vector<int> &cells) const;

    void drawMap() const;
    void drawHoveredCellHighlight() const;
    void drawSelectionRectangle() const;
    void drawPastePreviewGhost() const;
    void drawStartCells() const;
    void drawGrid() const;
    void drawAxes() const;
    int getActiveCursorSize() const;
    int getNormalizedCursorSize() const;
    void getBrushTileBounds(int centerTileX, int centerTileY, int &brushStartTileX, int &brushStartTileY, int &brushEndTileX, int &brushEndTileY) const;
    void modifyTile(int posX, int posY, int tileID);
    void modifyStartCell(int posX, int posY, int startCellID);
    void toggleMarkerCell(int posX, int posY, std::vector<int> &cells);
    void drawMarkerCells(const std::vector<int> &cells, TextureID textureId) const;
    void normalizeModifications();
    bool tryGetTileFromMouseCoords(const cPoint &coords, int &tileX, int &tileY) const;
    bool tryGetEditableTileFromMouseCoords(const cPoint &coords, int &tileX, int &tileY) const;
    bool isEditableTile(int tileX, int tileY) const;
    void updateSelectionFromTiles(int startTileX, int startTileY, int endTileX, int endTileY);
    void copySelectionToClipboard();
    void cutSelectionToClipboard();
    void pasteClipboardAtMouseCursor();

    bool saveMap(bool backup = false) const;
    std::unique_ptr<GuiBar> m_selectBar;
    std::unique_ptr<GuiBar> m_topologyBar;
    std::unique_ptr<GuiBar> m_startCellBar;
    std::unique_ptr<GuiBar> m_symmetricBar;
    GuiBar* m_currentBar = nullptr;
    std::unique_ptr<Matrix<int>> m_mapData;
    std::unique_ptr<GuiButtonGroup> m_selectGroup;
    std::unique_ptr<GuiButtonGroup> m_topologyGroup;
    std::unique_ptr<GuiButtonGroup> m_startCellGroup;
    std::unique_ptr<GuiButtonGroup> m_symmetricGroup;

    std::unique_ptr<cEditorUndoRedoHistory> m_undoRedo;
    std::unique_ptr<cEditorCam> m_editorCam;

    cRectangle mapSizeArea;

    // map modification 
    int idTerrainToMapModif = -1;
    static constexpr int PLACE_BLOOM = -2; // idTerrainToMapModif values that place a marker instead of terrain
    static constexpr int PLACE_FIELD = -3;
    int idStartCellPlayer = -1;
    int m_cursorSize = 1;
    int m_topologyCursorSize = 1;
    bool m_displayGrid = false;
    bool m_displayAxes = false;
    bool m_hasChanged = false;
    bool m_displayPastPreview = false;

    bool m_hasSelection = false;
    bool m_isDraggingSelection = false;
    int m_selectionStartTileX = 1;
    int m_selectionStartTileY = 1;
    int m_selectionEndTileX = 1;
    int m_selectionEndTileY = 1;
    int m_selectionAnchorTileX = 1;
    int m_selectionAnchorTileY = 1;

    std::vector<std::vector<int>> m_clipboardTiles;

    // startCell positions 
    std::array<cPoint,5> startCells;
    // bloom and field cells: raw index into the bordered map (as in the skirmish ini), RANDOM_CELL for a random one
    std::vector<int> m_blooms;
    std::vector<int> m_fields;
private:
    // graphics resolved once from the pack
    TextureID m_texTerrainSpice = TextureID::invalid;
    TextureID m_texTerrainSand = TextureID::invalid;
    TextureID m_texTerrainMountain = TextureID::invalid;
    TextureID m_texTerrainRock = TextureID::invalid;
    TextureID m_texTerrainSpicehill = TextureID::invalid;
    TextureID m_texTerrainHill = TextureID::invalid;
    TextureID m_texBloom = TextureID::invalid;
    TextureID m_texField = TextureID::invalid;
    std::array<TextureID, 5> m_texStartPosition{};
};
