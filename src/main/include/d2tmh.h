/*
 * d2tmh.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

// this file is the uber header file. Responsible for including all proper header files in the
// right order
#ifndef D2TMH_H
#define D2TMH_H

//#include "../memoryleakfinder/debug_new.h"

#include "system.h"
#include "allegroh.h"
#include "data.h"
#include "definitions.h"
#include "structs.h"
#include "global.h"

#include "../movie/cMoviePlayer.h"

#include "gameobjects.h"
#include "gamelogic.h"

// Player related
#include "../player/playerh.h"
#include "../ai.h"

#include "gui.h"
#include "utils.h"
#include "drawers.h"
#include "managers.h"

// Extern references & variable declarations
#include "d2tmc.h"

#endif
