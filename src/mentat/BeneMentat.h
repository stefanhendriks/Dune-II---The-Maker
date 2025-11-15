#pragma once

#include "AbstractMentat.h"

class cTextDrawer;
/**
 * This is the mentat that is used for house selection. (Bene Geserit)
 */
class BeneMentat : public AbstractMentat {
public:
    BeneMentat(GameContext* ctx);
    void think() override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
private:
    void onYesButtonPressed();
    void onNoButtonPressed();

    void draw_mouth() override;
    void draw_eyes() override;
    void draw_other() override;
    void draw() override;
};
