/**
 * @file BeneMentat.h
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

#include "AbstractMentat.h"

class cTextDrawer;
struct s_DataCampaign;

/**
 * This is the mentat that is used for house selection. (Bene Geserit)
 */
class BeneMentat : public AbstractMentat {
public:
    BeneMentat(GameContext* ctx, s_DataCampaign* dataCampaign);
    void think() override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
private:
    void onYesButtonPressed();
    void onNoButtonPressed();

    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    s_DataCampaign* m_dataCampaign;
};
