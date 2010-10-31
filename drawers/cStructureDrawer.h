/*
 * cStructureDrawer.h
 *
 *  Created on: 23-okt-2010
 *      Author: Stefan
 *
 *  Responsible for drawing structures
 *
 */

#ifndef CSTRUCTUREDRAWER_H_
#define CSTRUCTUREDRAWER_H_

class cStructureDrawer {
	public:
		cStructureDrawer();
		~cStructureDrawer();

		void drawStructuresFirstLayer();
		void drawStructuresSecondLayer();
		void drawStructuresHealthBars();

	protected:
		void drawStructureHealthBar(int iStructure);
		void drawStructuresForLayer(int layer);
		void drawRectangeOfStructure(cAbstractStructure * theStructure, int color);

	private:

};

#endif /* CSTRUCTUREDRAWER_H_ */
