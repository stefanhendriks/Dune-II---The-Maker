#pragma once

class s_GameEvent;

class IAnimationComponent {
public:
    virtual void specialAnimation() = 0;
};

class IStartAnimatingComponent {
public:
    virtual void startAnimating() = 0;
};

class ISpecialFastComponent {
public:
    virtual void specialThinkFast() = 0;
};

class INotifygameEvent {
public:
    virtual void notifyGameEvent(const s_GameEvent &event) = 0;

};

class ThinkGuard {
public:
    virtual void thinkGuard() = 0;
};

class ThinkSlow {
public:
    virtual void thinkSlow() = 0;
};

/*
class AttackComponent {
public:
    void think_attack() {
        // logique d'attaque
    }
};

class AnimComponent {
public:
    void think_attack() {
        // logique d'attaque
    }
};

class RadarAnimationComponent {
public:
    int TIMER_flag = 0;
    int iFrame = 0;

    void thinkFast() {
        TIMER_flag++;
        if (TIMER_flag > 54) {
            iFrame++;
            if (iFrame > 7) {
                iFrame = 0;
            }
            TIMER_flag = 0;
        }
    }
};

*/