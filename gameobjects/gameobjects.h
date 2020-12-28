/*
 * gameobjects.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

#ifndef GAMEOBJECTS_H2_
#define GAMEOBJECTS_H2_

// GAME
#include "../cGame.h"

// MAP
#include "../map/maph.h"							// map data

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

// Mentats
#include "mentat/cButtonCommand.h"
#include "mentat/cYesButtonCommand.h"
#include "mentat/cNoButtonCommand.h"
#include "mentat/cProceedButtonCommand.h"
#include "mentat/cRepeatButtonCommand.h"
#include "mentat/cAbstractMentat.h"
#include "mentat/cBeneMentat.h"
#include "mentat/cAtreidesMentat.h"
#include "mentat/cOrdosMentat.h"
#include "mentat/cHarkonnenMentat.h"


#endif /* GAMEOBJECTS_H_ */
