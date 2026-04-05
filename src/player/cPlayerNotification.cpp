#include "cPlayerNotification.h"

#include <cassert>


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
    : m_msg(msg), m_TIMER((1000/5) * 7), m_initialDuration(m_TIMER), m_type(type)
{
}

void cPlayerNotification::thinkFast()
{
    m_TIMER--;
}

const std::string &cPlayerNotification::getMessage() const
{
    return m_msg;
}

Color cPlayerNotification::getColor() const
{
    // @Mira regression on color
    //bool justStarted = (m_initialDuration - m_TIMER) < 500;
    static const Color neutralColor = Color::white(); //Color{255, 255, 255,255}; // white
    static const Color priorityColor = Color::yellow(); //Color{255, 207, 41,255}; // yellow
    static const Color badColor = Color::red(); //{255, 0, 0,255}; // red
    switch (m_type) {
        case NEUTRAL:
            return /*justStarted ? game.getColorFadeSelected(neutralColor) :*/ neutralColor;
        case PRIORITY:
            return /*justStarted ? game.getColorFadeSelected(priorityColor) :*/ priorityColor;
        case BAD:
            return /*justStarted ? game.getColorFadeSelected(badColor) :*/ badColor;
    }
    return neutralColor;
}
