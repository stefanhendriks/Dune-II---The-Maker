#include "../include/d2tmh.h"

cPlayerNotification::cPlayerNotification(std::string msg, eNotificationType type) {
    this->msg = msg;
    this->type = type;
    TIMER = (1000/5) * 7;
    initialDuration = TIMER;
}

void cPlayerNotification::thinkFast() {
    TIMER--;
}

std::string & cPlayerNotification::getMessage() {
    return msg;
}

int cPlayerNotification::getColor() {
    bool justStarted = (initialDuration - TIMER) < 500;
    static int neutralColor = makecol(255, 255, 255); // white
    static int priorityColor = makecol(255, 207, 41); // yellow
    static int badColor = makecol(255, 0, 0); // red
    switch (type) {
        case NEUTRAL:
            return justStarted ? game.getColorFadeSelected(neutralColor) : neutralColor;
        case PRIORITY:
            return justStarted ? game.getColorFadeSelected(priorityColor) : priorityColor;
        case BAD:
            return justStarted ? game.getColorFadeSelected(badColor) : badColor;
    }
}
