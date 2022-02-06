// this file is the uber header file. Responsible for including all proper header files in the
// right order
#pragma once

#include "data.h"
#include "definitions.h"
#include "structs.h"
#include "gameobjects.h"
#include "gamestates.h"
#include "gamelogic.h"
#include "utils/utilsh.h"

// Player related
#include "../player/playerh.h"

#include "gui.h"
#include "drawers/cAllegroDrawer.h"
#include "managers/cDrawManager.h"
#include "managers/cInteractionManager.h"

// Extern references & variable declarations
#include "d2tmc.h"
