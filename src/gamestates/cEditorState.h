#pragma once

#include "cGameState.h"
#include "utils/cMatrix.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"

#include <memory>
#include <array>

class Texture;
class Graphics;
class cGame;
class s_PreviewMap;
class GuiBar;
class GuiButtonGroup;

class cEditorState : public cGameState {
public:
    explicit cEditorState(cGame &theGame, GameContext* ctx);
    ~cEditorState() override;

    void thinkFast() override;
    void draw() const override;
    void loadMap(s_PreviewMap* map);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
private:
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
    void modifySymmetricArea(Direction dir);

    void drawMap() const;
    void drawStartCells() const;
    void modifyTile(int posX, int posY, int tileID);
    void modifyStartCell(int posX, int posY, int startCellID);
    void normalizeModifications();
    void clampCameraXToMapBounds();
    void clampCameraYToMapBounds();
    void zoomAtMapPosition(int screenX, int screenY, ZoomDirection direction);
    void updateVisibleTiles();

    void saveMap() const;
    std::unique_ptr<GuiBar> m_selectBar;
    std::unique_ptr<GuiBar> m_topologyBar;
    std::unique_ptr<GuiBar> m_startCellBar;
    std::unique_ptr<GuiBar> m_symmetricBar;
    GuiBar* m_currentBar = nullptr;
    std::unique_ptr<Matrix<int>> m_mapData;
    Graphics *m_gfxdata, *m_gfxeditor;

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

    // startCell positions 
    std::array<cPoint,5> startCells;
};
