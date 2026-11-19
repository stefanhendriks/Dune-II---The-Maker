/**
 * @file cSideBarDrawer.h
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
#include "cBuildingListDrawer.h"
#include "cTextDrawer.h"
class cKeyboardEvent;
#include "observers/cInputObserver.h"
struct s_MouseEvent;

#include <memory>

class Texture;
class cPlayer;
class GameContext;
class Graphics;
class SDLDrawer;
class cInfoContext;
class cMouse;
struct sGameServices;

class cSideBarDrawer : cInputObserver {
public:
    explicit cSideBarDrawer(GameContext *ctx, cPlayer *player, sGameServices *services);
    ~cSideBarDrawer() override;

    void serviceInit(sGameServices* services);

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
    Graphics *m_gfxinter;
    SDLDrawer* m_sdlDrawer;
    GameContext* m_ctx = nullptr;
    cInfoContext* m_infos = nullptr;
    cMouse* m_mouse = nullptr;
    sGameServices* m_services = nullptr;
    cBuildingListDrawer m_buildingListDrawer;
    cSideBar *m_sidebar;
    Color m_sidebarColor;

    void createCandyBar();
    void createPlayerCandyBar();

    void drawPowerUsage() const;
    void drawCreditsUsage();
    std::unique_ptr<Texture> m_candyBarBall;
    std::unique_ptr<Texture> m_candyBarPiece;
    std::unique_ptr<Texture> m_candyHorizonBar;
    Texture *m_candiBarRenderer= nullptr;
private:
    // graphics resolved once from the pack
    SurfaceID m_surfBmpGeraldCandybarBall = SurfaceID::invalid;
    SurfaceID m_surfBmpGeraldCandybarPiece = SurfaceID::invalid;
    SurfaceID m_surfHorizontalCandybar = SurfaceID::invalid;
    TextureID m_texBmpGeraldIconlistBackground = TextureID::invalid;
    TextureID m_texBmpGeraldSidebarPiece = TextureID::invalid;
    TextureID m_texBmpSelectHouseAtreides = TextureID::invalid;
    TextureID m_texBmpSelectHouseHarkonnen = TextureID::invalid;
    TextureID m_texBmpSelectHouseOrdos = TextureID::invalid;
    TextureID m_texBmpSelectHouseSardaukar = TextureID::invalid;
    TextureID m_texBmpGeraldCandybarTop = TextureID::invalid;
    TextureID m_texBmpGeraldCandybarBottom = TextureID::invalid;
    TextureID m_texIconPowerHigh = TextureID::invalid;
    TextureID m_texIconPower = TextureID::invalid;
    TextureID m_texIconSolarisFull = TextureID::invalid;
    TextureID m_texIconSolarisLow = TextureID::invalid;
    TextureID m_texIconSolaris = TextureID::invalid;
};
