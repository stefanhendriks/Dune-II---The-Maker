#pragma once

#include "cGameState.h"
#include "utils/cMatrix.h"
#include "controls/cKeyboardEvent.h"
#include "sMouseEvent.h"
#include "utils/cRectangle.h"

#include <memory>
#include <array>
#include <string>

class Texture;
class Graphics;

struct s_PreviewMap;
class GuiBar;
class GuiButtonGroup;
class cTextDrawer;
class cGameInterface;

class cEditorState : public cGameState {
public:
    explicit cEditorState(sGameServices* services);
    ~cEditorState() override;

    void thinkFast() override;
    void draw() const override;
    void loadMap(s_PreviewMap* map);
    void setCursorSize(int value);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
private:
    Graphics *m_gfxdata = nullptr;
    Graphics *m_gfxeditor = nullptr;
    cGameSettings *m_settings = nullptr;
    cGameInterface* m_interface = nullptr;
    cTextDrawer *m_textDrawer = nullptr;
    std::string m_mapName, m_mapAuthor, m_mapDescription;
    enum class ZoomDirection : char {
        zoomIn,
        zoomOut
    };
    enum class Direction : char {
        top,
        bottom,
        left,
        right
    };
    void populateTopologyBar();
    void populateStartCellBar();
    void populateSelectBar();
    void populateSymmetricBar();
    void setCurrentBar(GuiBar* bar);
    void modifySymmetricArea(Direction dir);

    void drawMap() const;
    void drawHoveredCellHighlight() const;
    void drawStartCells() const;
    void drawGrid() const;
    void drawAxes() const;
    int getActiveCursorSize() const;
    int getNormalizedCursorSize() const;
    void getBrushTileBounds(int centerTileX, int centerTileY, int &brushStartTileX, int &brushStartTileY, int &brushEndTileX, int &brushEndTileY) const;
    void modifyTile(int posX, int posY, int tileID);
    void modifyStartCell(int posX, int posY, int startCellID);
    void normalizeModifications();
    void clampCameraXToMapBounds();
    void clampCameraYToMapBounds();
    void zoomAtMapPosition(int screenX, int screenY, ZoomDirection direction);
    void updateVisibleTiles();

    void saveMap(bool backup = false) const;
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

    cRectangle mapSizeArea;
    int tileLenSize = 16;

    // Coordinates of the top left corner of the screen on the map (in pixels)
    int cameraX = 0;
    int cameraY = 0;
    // Visible tile range, here to avoid creating them each frame
    int startX = 0, startY = 0;
    size_t endX = 0, endY = 0;
    size_t tilesAcross = 0, tilesDown = 0;

    // map modification 
    int idTerrainToMapModif = -1;
    int idStartCellPlayer = -1;
    int m_cursorSize = 1;
    int m_topologyCursorSizeSentinel = 1;
    bool m_displayGrid = false;
    bool m_displayAxes = false;
    bool m_hasChanged = false;

    // startCell positions 
    std::array<cPoint,5> startCells;
};
