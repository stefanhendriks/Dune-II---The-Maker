/**
 * @file eMouseTile.h
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

// The mouse pictures the game can show. cMouse resolves each one to its sprite in the graphics pack once.
enum class eMouseTile {
    NORMAL,
    DOWN,
    RIGHT,
    LEFT,
    UP,
    MOVE,
    RALLY,
    ATTACK,
    REPAIR,
    FORBIDDEN,
    PICK,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
};
