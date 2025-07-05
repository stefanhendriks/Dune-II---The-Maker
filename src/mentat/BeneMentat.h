#pragma once

#include "AbstractMentat.h"

class cTextDrawer;
/**
 * This is the mentat that is used for house selection. (Bene Geserit)
 */
class BeneMentat : public AbstractMentat {
public:
    BeneMentat();
    // void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void think() override;
private:
    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
    // void interact() override;
    cTextDrawer *textDrawer;
};
