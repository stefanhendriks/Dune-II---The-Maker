#pragma once

#include "cBuildingListDrawer.h"
#include "cTextDrawer.h"
#include "controls/cKeyboardEvent.h"
#include "observers/cInputObserver.h"
#include "sMouseEvent.h"

class Texture;
class cPlayer;
class GameContext;

class cSideBarDrawer : cInputObserver {
public:
    explicit cSideBarDrawer(GameContext *ctx, cPlayer *player);
    ~cSideBarDrawer() override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw();

    cBuildingListDrawer *getBuildingListDrawer() {
        return &m_buildingListDrawer;
    }

    void setPlayer(cPlayer *pPlayer);

protected:
    void drawBuildingLists();
    void drawCapacities();

    void drawCandybar();

    void drawMinimap();

private:
    cPlayer *m_player;
    GameContext *m_ctx;
    cBuildingListDrawer m_buildingListDrawer;
    cSideBar *m_sidebar;
    Color m_sidebarColor;

    void createCandyBar();

    void drawPowerUsage() const;
    void drawCreditsUsage();
    Texture *candyBarBall=nullptr;
    Texture *candyBarPiece=nullptr;
    Texture *candyHorizonBar=nullptr;
    Texture *candiBarRenderer= nullptr;
};
