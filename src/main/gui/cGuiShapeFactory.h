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
		cGuiShapeFactory(cScreenResolution * theScreenResolution);
		~cGuiShapeFactory();

		cGuiButton * createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel);

		cGuiButton * createGrayButton(cRectangle * rect, std::string theLabel);

		cMainMenuDialog * createMainMenuDialog();

	protected:

	private:
		int defaultGreyColorInner;
		int defaultGreyColorLighterBorder;
		int defaultGreyColorDarkerBorder;
		cScreenResolution * screenResolution;
};

#endif /* CGUISHAPEFACTORY_H_ */
