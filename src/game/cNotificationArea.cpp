/**
 * @file cNotificationArea.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cNotificationArea.h"

#include "drawers/cTextDrawer.h"
#include "drawers/SDLDrawer.hpp"

#include <algorithm>
#include <cassert>

void cNotificationArea::setDrawer(SDLDrawer* renderer)
{
    m_drawer = renderer;
    assert(m_drawer != nullptr);
}

void cNotificationArea::clear()
{
    m_notifications.clear();
}

void cNotificationArea::addNotification(const std::string &msg, eNotificationType type)
{
    m_notifications.push_back(cPlayerNotification(msg, type));
    std::sort(m_notifications.begin(), m_notifications.end(), [](const cPlayerNotification &lhs, const cPlayerNotification &rhs) {
        return lhs.getTimer() > rhs.getTimer();
    });
}

void cNotificationArea::thinkFast()
{
    for (auto &notification : m_notifications) {
        notification.thinkFast();
    }

    m_notifications.erase(
        std::remove_if(
            m_notifications.begin(),
            m_notifications.end(),
            [](cPlayerNotification notification) {
                return !notification.isVisible();
            }),
        m_notifications.end());
}

void cNotificationArea::draw(const cTextDrawer *textDrawer, int x, int y) const
{
    if (!textDrawer) return;
    if (m_notifications.size() == 0) return;

    auto height = 20 * m_notifications.size();
    m_drawer->renderRectFillColor(cRectangle(x-5,y-5-height+20, 500+5, 5+height), {0, 0, 0, 128}, 64); // clear the area before drawing notifications
    for (const auto &notification : m_notifications) {
        textDrawer->drawText(x, y, notification.getColor(), notification.getMessage());
        y -= 20;
    }    
}

std::vector<cPlayerNotification> &cNotificationArea::getNotifications()
{
    return m_notifications;
}

const std::vector<cPlayerNotification> &cNotificationArea::getNotifications() const
{
    return m_notifications;
}