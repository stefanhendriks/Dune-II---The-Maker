/*
 * cGuiShapeFactory.h
 *
 *  Created on: 26-aug-2011
 *      Author: Stefan
 */

#ifndef CGUISHAPEFACTORY_H_
#define CGUISHAPEFACTORY_H_

class cGuiShapeFactory {

	public:
		cGuiShapeFactory();
		~cGuiShapeFactory();

		cGuiButton * createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel);

		cGuiButton * createGrayButton(cRectangle * rect, std::string theLabel);

	protected:

	private:

};

#endif /* CGUISHAPEFACTORY_H_ */
