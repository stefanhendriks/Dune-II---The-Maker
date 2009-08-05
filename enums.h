/*
 * enums.h
 *
 *  Created on: Aug 5, 2009
 *      Author: Stefan
 */

#ifndef ENUMS_H_
#define ENUMS_H_

enum eLogComponent {
	COMP_UNITS,
	COMP_STRUCTURES,
	COMP_GAMEINI,
	COMP_SCENARIOINI,
	COMP_PARTICLE,
	COMP_BULLET,
	COMP_AI,
	COMP_SIDEBAR,
	COMP_MAP,
	COMP_NONE,
	COMP_INIT,
	COMP_ALLEGRO,	/** Use for allegro specific calls **/
	COMP_SETUP
};

enum eLogOutcome {
	OUTC_SUCCESS,
	OUTC_FAILED,
	OUTC_NONE,
	OUTC_UNKNOWN,
	OUTC_IGNOREME /** will not be printed **/
};

enum eLogLevel {
	LOG_INFO,
	LOG_TRACE,
	LOG_WARN,
	LOG_ERROR,
	LOG_FATAL
};


#endif /* ENUMS_H_ */
