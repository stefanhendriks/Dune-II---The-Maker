/**
 * Mother object of all gui things alive
 */
#ifndef GUIELEMENT_H_
#define GUIELEMENT_H_

class GuiElement {
	public:
		virtual ~GuiElement() = 0;
		virtual void draw() = 0;
};

#endif /* GUIELEMENT_H_ */
