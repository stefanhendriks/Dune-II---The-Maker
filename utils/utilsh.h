/*
 * utilsh.h
 *
 *  Created on: 7-nov-2010
 *      Author: Stefan
 */

#ifndef UTILSH_H_
#define UTILSH_H_

// TODO: Rethink this, perhaps other global dir is needed
#include "common.h"					// commonly used functions
#include "../ini.h"							// INI loading

#include "cCellCalculator.h"			    /** math with cells , with handy references to structures and units **/
#include "cSeedMap.h"
#include "cSeedMapGenerator.h"
#include "cSimpleCalculator.h"
#include "cStructureUtils.h"

#include "cHitpointCalculator.h"			/** math for hitpoints **/
#include "cTimeManager.h"
#include "cUpgradeUtils.h"
#include "cMapUtils.h"
#include "cPlayerUtils.h"
#include "cListUtils.h"
#include "cMouseUtils.h"

#endif /* UTILSH_H_ */
