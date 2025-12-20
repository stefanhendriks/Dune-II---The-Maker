#pragma once

#include "cGameState.h"
#include "utils/cMatrix.h"
#include "controls/cKeyboardEvent.h"
#include "drawers/cTextDrawer.h"
#include "sMouseEvent.h"

#include <memory>

class Texture;
class Graphics;
class cGame;
class s_PreviewMap;
class GuiBar;

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
    void populateModifBar();
    void populateSelectBar();

    //s_PreviewMap* m_map=nullptr;
    std::unique_ptr<GuiBar> m_selectBar;
    std::unique_ptr<GuiBar> m_modifBar;
    std::unique_ptr<Matrix<int>> m_mapData;
    Graphics *m_gfxinter, *m_gfxeditor;
};
