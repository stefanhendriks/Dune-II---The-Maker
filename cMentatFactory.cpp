/**
	Responsible for creating and destroying the mentat reference

*/
#include "d2tmh.h"

cMentatFactory::cMentatFactory() {
	type = NOT_DEFINED;
}

void cMentatFactory::deleteCurrent() {
	switch (type) {
		case ATREIDES:
			delete (cAtreidesMentat *)Mentat;
			break;
		case HARKONNEN:
			delete (cHarkonnenMentat *)Mentat;
			break;
		case ORDOS:
			delete (cOrdosMentat *)Mentat;
			break;
		case BENEGES:
			delete (cBenegesMentat *)Mentat;
			break;
	}
}

void cMentatFactory::create(int mentatType) {
	if (type != NOT_DEFINED) {
		if (mentatType != type) {
			deleteCurrent();
		} else {
			// already created
			return; 
		}
	}
	/** It is safe to create a new Mentat now **/	
	switch (mentatType) {
		case ATREIDES:
			Mentat = new cAtreidesMentat;
			break;
		case HARKONNEN:
			Mentat = new cHarkonnenMentat;
			break;
		case ORDOS:
			Mentat = new cOrdosMentat;
			break;
		case BENEGES:
			Mentat = new cBenegesMentat;
			break;
	}
	type = mentatType;
}