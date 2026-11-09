/**
 * @file cInputObserver.h
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

class cKeyboardEvent;
struct s_MouseEvent;

class cInputObserver {
public:
    virtual ~cInputObserver() = default;

    virtual void onNotifyMouseEvent(const s_MouseEvent &event) = 0;
    virtual void onNotifyKeyboardEvent(const cKeyboardEvent &event) = 0;
};
