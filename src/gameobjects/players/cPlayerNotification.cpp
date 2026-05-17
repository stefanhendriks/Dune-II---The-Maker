#include "cPlayerNotification.h"

#include "include/cAssert.h"


std::string eNotificationTypeString(const eNotificationType &type)
{
    switch (type) {
        case eNotificationType::NEUTRAL:
            return "NEUTRAL";
        case eNotificationType::PRIORITY:
            return "PRIORITY";
        case eNotificationType::BAD:
            return "BAD";
        case eNotificationType::OTHER:
            return "OTHER";
        default:
            d2tm_assert(false && "Unknown eNotificationType?");
            break;
    }
    return {};
}

cPlayerNotification::cPlayerNotification(const std::string &msg, eNotificationType type)
    : m_msg(msg), m_TIMER((1000/5) * 7), m_type(type)
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
    static const Color neutralColor = Color::White;
    static const Color priorityColor = Color::Yellow;
    static const Color badColor = Color::Red;
    static const Color otherColor = Color::Green;
    switch (m_type) {
        case NEUTRAL:
            return neutralColor;
        case PRIORITY:
            return priorityColor;
        case BAD:
            return badColor;
        case OTHER:
            return otherColor;
    }
}
