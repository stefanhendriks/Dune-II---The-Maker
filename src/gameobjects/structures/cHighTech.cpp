#include "cHighTech.h"

#include "definitions.h"

cHighTech::cHighTech()
{
    // other variables (class specific)
    animDir = eAnimationDirection::ANIM_NONE;
}

int cHighTech::getType() const
{
    return HIGHTECH;
}

void cHighTech::thinkFast()
{
    // last but not least, think like our abstraction
    cAbstractStructure::thinkFast();
}

void cHighTech::think_animation_unitDeploy()
{
    if (!isAnimating()) return; // do nothing when not animating
    TIMER_flag++;
    int waitLimit = animDir == eAnimationDirection::ANIM_SPAWN_UNIT ? 120 : 30;
    if (TIMER_flag > waitLimit) {

        switch (animDir) {
            case eAnimationDirection::ANIM_OPEN:
                iFrame++;
                if (iFrame > 5) {
                    iFrame = 5;
                    animDir = eAnimationDirection::ANIM_SPAWN_UNIT;
                }
                break;
            case eAnimationDirection::ANIM_SPAWN_UNIT:
                animDir = eAnimationDirection::ANIM_CLOSE; // and close again
                break;
            case eAnimationDirection::ANIM_CLOSE:
                iFrame--;
                if (iFrame < 1) {
                    // no longer animating
                    animDir = eAnimationDirection::ANIM_NONE;
                    setAnimating(false);
                }
                break;
            default:
                break;
        }

        TIMER_flag = 0;
    }

}

// Specific Animation thinking (flag animation OR its deploy animation)
void cHighTech::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_animation_unitDeploy();
}

void cHighTech::think_guard()
{

}

void cHighTech::startAnimating()
{
    animDir = isAnimating() ? eAnimationDirection::ANIM_OPEN : eAnimationDirection::ANIM_NONE;
}
