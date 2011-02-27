/*
 * cGuiDrawer.h
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#ifndef CGUIDRAWER_H_
#define CGUIDRAWER_H_

class cGuiDrawer {
	public:
		cGuiDrawer();
		virtual ~cGuiDrawer();

		void drawShape(cGuiShape * shapeToDraw);
};

#endif /* CGUIDRAWER_H_ */
