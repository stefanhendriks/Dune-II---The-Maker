#ifndef CGUIBUTTON_H_
#define CGUIBUTTON_H_

class cGuiButton : public cGuiShape {

	public:
		cGuiButton(cRectangle * rect, std::string theLabel);
		cGuiButton(int x, int y, int width, int height, std::string theLabel);
		virtual ~cGuiButton();

		std::string * getLabel() {
			return label;
		}

		BITMAP * getBitmap() { return bitmap; }
		void setBitmap(BITMAP * value) { bitmap = value; }

		bool shouldDrawPressedWhenMouseHovers() { return drawPressedWhenMouseHovers; }
		void setDrawPressedWhenMouseHovers(bool value) { drawPressedWhenMouseHovers = value; }

	private:
		std::string * label;
		bool drawPressedWhenMouseHovers;
		BITMAP * bitmap;
};

#endif /* CGUIBUTTON_H_ */
