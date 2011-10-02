/*
 * d2tmh.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

// this file is the uber header file. Responsible for including all proper header files in the
// right order
// TODO: this should actually dissapear so that all the dependencies are included at their appropiate places
// this way it introduces some global 'dependency state' somehow, which is a bad thing.
#ifndef D2TMH_H
#define D2TMH_H

#include "system.h"
#include "allegroh.h"
#include "data.h"
#include "definitions.h"
#include "structs.h"
#include "global.h"

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
