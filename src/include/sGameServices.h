/**
 * @file sGameServices.h
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

class GameContext;
class cGameObjectContext;
class cInfoContext;
class cGameSettings;
class cLog;
class cMapCamera;
class cStructureUtils;
class cEventEmitter;
class cDrawManager;
class cRectangle;

struct sGameServices
{
    GameContext *ctx = nullptr;
    cGameObjectContext *objects = nullptr;
    cInfoContext *info = nullptr;
    cGameSettings *settings = nullptr;
    cMapCamera *mapCamera = nullptr;
    cStructureUtils *structureUtils = nullptr;
    cEventEmitter *eventEmitter = nullptr;
    cDrawManager *drawManager = nullptr;
    cRectangle *mapViewport = nullptr;
};
