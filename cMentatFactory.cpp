/**
	Responsible for creating and destroying the mentat reference

*/
#include "d2tmh.h"

cMentatFactory::cMentatFactory() {
	type = -1;
}

void cMentatFactory::deleteCurrent() {
	switch (type) {
		case ATREIDES:
			delete (cAtreidesMentat *)Mentat;
			break;
		case HARKONNEN:
			delete (cAtreidesMentat *)Mentat;
			break;
		case ORDOS:
			delete (cAtreidesMentat *)Mentat;
			break;
		case BENEGES:
			delete (cAtreidesMentat *)Mentat;
			break;
	}
}

void cMentatFactory::create(int mentatType) {
	if (type > -1) {
		if (mentatType != type) {
			deleteCurrent();
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