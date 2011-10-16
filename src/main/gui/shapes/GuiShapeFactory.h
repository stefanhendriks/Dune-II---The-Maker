/**
 * This factory is responsible for creating gui shapes or their children (button, etc).
 *
 */
#ifndef CGUISHAPEFACTORY_H_
#define CGUISHAPEFACTORY_H_

#include <string>

#include "../../gameobjects/ScreenResolution.h"

#include "../GuiColors.h"
#include "GuiButton.h"

class GuiShapeFactory {

	public:
		GuiShapeFactory(ScreenResolution * theScreenResolution);
		~GuiShapeFactory();

		GuiButton * createButtonWithBitmap(int x, int y, BITMAP * bmp, std::string theLabel);

		GuiButton * createGrayButton(Rectangle * rect, std::string theLabel);

	protected:

	private:
		GuiColors guiColors;
		ScreenResolution * screenResolution;
};

#endif /* CGUISHAPEFACTORY_H_ */
