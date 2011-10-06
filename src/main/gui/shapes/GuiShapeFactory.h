/**
 * This factory is responsible for creating gui shapes or their children (button, etc).
 *
 */
#ifndef CGUISHAPEFACTORY_H_
#define CGUISHAPEFACTORY_H_

#include <string>

class GuiShapeFactory {

	public:
		GuiShapeFactory(cScreenResolution * theScreenResolution);
		~GuiShapeFactory();

		GuiButton * createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel);

		GuiButton * createGrayButton(Rectangle * rect, std::string theLabel);

		MainMenuDialog * createMainMenuDialog();

	protected:

	private:
		int defaultGreyColorInner;
		int defaultGreyColorLighterBorder;
		int defaultGreyColorDarkerBorder;
		cScreenResolution * screenResolution;
};

#endif /* CGUISHAPEFACTORY_H_ */
