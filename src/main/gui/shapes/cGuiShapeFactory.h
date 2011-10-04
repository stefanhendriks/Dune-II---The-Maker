/**
 * This factory is responsible for creating gui shapes or their children (button, etc).
 *
 */
#ifndef CGUISHAPEFACTORY_H_
#define CGUISHAPEFACTORY_H_

#include <string>

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
