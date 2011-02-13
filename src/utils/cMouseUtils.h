/*
 * cMouseUtils.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef CMOUSEUTILS_H_
#define CMOUSEUTILS_H_

class cMouseUtils {
	public:
		cMouseUtils();
		cMouseUtils(cMouse *theMouse);
		~cMouseUtils();

		bool isMouseOverRectangle(int x, int y, int width, int height);

	protected:

	private:
		cMouse *mouse;
};

#endif /* CMOUSEUTILS_H_ */
