#include "cPlayerNotification.h"

#include "d2tmc.h"


std::string eNotificationTypeString(const eNotificationType &type)
{
    switch (type) {
        case eNotificationType::NEUTRAL:
            return "NEUTRAL";
        case eNotificationType::PRIORITY:
            return "PRIORITY";
        case eNotificationType::BAD:
            return "BAD";
        default:
            assert(false && "Unknown eNotificationType?");
            break;
    }
    return {};
}

cPlayerNotification::cPlayerNotification(const std::string &msg, eNotificationType type)
    : msg(msg), TIMER((1000/5) * 7), initialDuration(TIMER), type(type)
{
}

void cPlayerNotification::thinkFast()
{
    TIMER--;
}

const std::string &cPlayerNotification::getMessage() const
{
    return msg;
}

Color cPlayerNotification::getColor() const
{
    // @Mira regression on color
    //bool justStarted = (initialDuration - TIMER) < 500;
    static const Color neutralColor = Color{255, 255, 255,255}; // white
    static const Color priorityColor = Color{255, 207, 41,255}; // yellow
    static const Color badColor = Color{255, 0, 0,255}; // red
    switch (type) {
        case NEUTRAL:
            return /*justStarted ? game.getColorFadeSelected(neutralColor) :*/ neutralColor;
        case PRIORITY:
            return /*justStarted ? game.getColorFadeSelected(priorityColor) :*/ priorityColor;
        case BAD:
            return /*justStarted ? game.getColorFadeSelected(badColor) :*/ badColor;
    }
    return neutralColor;
}
