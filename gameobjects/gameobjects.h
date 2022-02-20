#ifndef GAMEOBJECTS_H2_
#define GAMEOBJECTS_H2_

#include "cFlag.h"

// GAME
#include "../cGame.h"

#include "structures/structuresh.h"

// units
// TODO: Refactor this big-... class into smaller ones (like structures)
#include "units/cUnit.h"

// TODO: Finish up these classes
#include "units/cAbstractUnit.h"					// Abstract Unit class (will replace cUnit.h eventually)
#include "units/cUnitFactory.h"				// Responsible for creation of Units
#include "units/cUnitUtils.h"				// Responsible for generic operations on units (ie finding a unit, etc)

// particles (explosion types, etc)
#include "particles/cParticle.h"                       // explosion animations and such

// bullet/projectiles
#include "projectiles/bullet.h"                         // bullets

// CONTROLS (mouse/keyboard)
#include "../controls/controlsh.h"

#endif /* GAMEOBJECTS_H_ */
