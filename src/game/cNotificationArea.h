/**
 * @file cNotificationArea.h
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

#pragma once

#include "gameobjects/players/cPlayerNotification.h"
#include <string>
#include <vector>

class cTextDrawer;
class SDLDrawer;

class cNotificationArea {
public:
    cNotificationArea() = default;
    void setDrawer(SDLDrawer* renderer);
    void clear();

    void addNotification(const std::string &msg, eNotificationType type);

    void thinkFast();

    void draw(const cTextDrawer *textDrawer, int x, int y) const;

    std::vector<cPlayerNotification> &getNotifications();
    const std::vector<cPlayerNotification> &getNotifications() const;

private:
    std::vector<cPlayerNotification> m_notifications;
    SDLDrawer* m_drawer = nullptr;
};