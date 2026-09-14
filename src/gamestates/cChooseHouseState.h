/**
 * @file cChooseHouseState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cGameState.h"
#include "controls/cKeyboardEvent.h"
#include "controls/sMouseEvent.h"
#include "utils/cRectangle.h"

struct SDL_Surface;
class Texture;
class GameContext;
class cGameInterface;
class cTextDrawer;
class Graphics;

class cChooseHouseState : public cGameState {
public:
    explicit cChooseHouseState(sGameServices* services);
    ~cChooseHouseState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;

private:
    cTextDrawer* m_textDrawer = nullptr;
    Graphics *m_gfxinter = nullptr;
    cGameInterface* m_interface = nullptr;
    cRectangle *backButtonRect;

    Texture *bmp_Dune;
    Texture *bmp_SelectYourHouseTitle;

    // TODO: abstract away the selectable houses
    Texture *bmp_HouseAtreides;
    Texture *bmp_HouseOrdos;
    Texture *bmp_HouseHarkonnen;
    Texture *bmp_HouseSardaukar;

    // Click areas of house emblems
    cRectangle houseAtreides;
    cRectangle houseOrdos;
    cRectangle houseHarkonnen;
    cRectangle houseSardaukar;

    int selectYourHouseXCentered;
    cPoint coords_Dune;
    cPoint coords_SelectYourHouseTitle;

    // Functions
    bool hoversOverBackButton;

    void onMouseLeftButtonClicked(const s_MouseEvent &event) const;

    void onMouseMoved(const s_MouseEvent &event);
};
