#pragma once
#include "definitions.h"
#include <memory>
#include "cAbstractStructure.h"
#include "StructComponent.hpp"


class Structure : public cAbstractStructure {
public:
    Structure(int type) 
        : cAbstractStructure()
        , type(type)
    {
        // type configuration
        // if (type == TURRET) thinkFast = std::make_unique<AttackComponent>();
        // if (type == WINDTRAP) anim = std::make_unique<AnimComponent>();
    }

    void think_animation() override
    {
        if (animation)
            animation->specialAnimation();
        cAbstractStructure::think_animation();
        cAbstractStructure::think_flag_new();
    }

    void thinkFast() override {
        if (specialThinkFast)
            specialThinkFast->specialThinkFast();
        cAbstractStructure::thinkFast();
    }

    void startAnimating() override {
        if (startAnim)
            startAnim->startAnimating();
    }

    int getType() const override {
        // retourne le type du bâtiment
        return type;
    }

    void thinkSlow() override {
        if (specialThinkSlow)
            specialThinkSlow->thinkSlow();
    }

    void think_guard() override {
        if (specialThinkGuard)
            specialThinkGuard->thinkGuard();
    }

    void onNotifyGameEvent(const s_GameEvent &event) override {
        if (notifyGameEvent)
            notifyGameEvent->notifyGameEvent(event);
    }

private:
    int type;
    std::unique_ptr<IAnimationComponent> animation;
    std::unique_ptr<IStartAnimatingComponent> startAnim;
    std::unique_ptr<ISpecialFastComponent> specialThinkFast;
    std::unique_ptr<INotifygameEvent> notifyGameEvent;
    std::unique_ptr<ThinkGuard> specialThinkGuard;
    std::unique_ptr<ThinkSlow> specialThinkSlow;
};