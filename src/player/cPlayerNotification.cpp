#include "cPlayerNotification.h"

#include "d2tmc.h"

#include <allegro/color.h>

std::string eNotificationTypeString(const eNotificationType &type) {
    switch (type) {
        case eNotificationType::NEUTRAL: return "NEUTRAL";
        case eNotificationType::PRIORITY: return "PRIORITY";
        case eNotificationType::BAD: return "BAD";
        default:
            assert(false && "Unknown eNotificationType?");
            break;
    }
    return {};
}

cPlayerNotification::cPlayerNotification(const std::string& msg, eNotificationType type)
  : msg(msg), TIMER((1000/5) * 7), initialDuration(TIMER), type(type) {
}

void cPlayerNotification::thinkFast() {
    TIMER--;
}

const std::string& cPlayerNotification::getMessage() const {
    return msg;
}

int cPlayerNotification::getColor() const {
    bool justStarted = (initialDuration - TIMER) < 500;
    static const int neutralColor = makecol(255, 255, 255); // white
    static const int priorityColor = makecol(255, 207, 41); // yellow
    static const int badColor = makecol(255, 0, 0); // red
    switch (type) {
        case NEUTRAL:
            return justStarted ? game.getColorFadeSelected(neutralColor) : neutralColor;
        case PRIORITY:
            return justStarted ? game.getColorFadeSelected(priorityColor) : priorityColor;
        case BAD:
            return justStarted ? game.getColorFadeSelected(badColor) : badColor;
    }
    return 0;
}
