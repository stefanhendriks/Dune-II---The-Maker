/**
 * @file cTellHouseState.h
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
#include "gameobjects/mentat/AbstractMentat.h"

#include <memory>

struct s_DataCampaign;
class cMouse;
class cIni;

class cTellHouseState : public cGameState {
public:
    cTellHouseState(sGameServices* services, cIni *ini, s_DataCampaign* dataCampaign);
    ~cTellHouseState() override;

    void thinkFast() override;
    void draw() const override;

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    eGameStateType getType() override;
    void prepareMentat(int house);

private:
    std::unique_ptr<AbstractMentat> m_mentat;
    int m_house = -1;
    s_DataCampaign* m_dataCampaign = nullptr;
    cMouse* m_mouse = nullptr;
    cIni* m_cIni = nullptr;
};