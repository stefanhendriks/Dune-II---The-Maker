#include "../include/d2tmh.h"


cTextDrawer::cTextDrawer(ALFONT_FONT *theFont) {
	assert(theFont);
	font = theFont;
	applyShadow=true;
    textColor = makecol(255, 255, 255);
}

cTextDrawer::cTextDrawer() : cTextDrawer(small_font) {
}

cTextDrawer::~cTextDrawer() {
	font = NULL; // do not delete, because we are not the owner of it
}

void cTextDrawer::drawTextWithTwoIntegers(int x, int y, const char * msg, int var1, int var2) const {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var1, var2);
	}
	alfont_textprintf(bmp_screen, font, x,y, textColor, msg, var1, var2);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, const char * msg, int var) const {
    drawTextWithOneInteger(x, y, textColor, msg, var);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, int color, const char * msg, int var) const {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var);
	}
	alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, const char *msg) const {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg);
	}
	alfont_textprintf(bmp_screen, font, x,y, color, msg);
}

void cTextDrawer::drawText(cPoint &coords, int color, const char *msg) const {
    drawText(coords.x, coords.y, color, msg);
}

void cTextDrawer::drawText(int x, int y, const char * msg) const {
    drawText(x, y, textColor, msg);
}

void cTextDrawer::drawTextCentered(const char *msg, int y) const{
	drawTextCentered(msg, y, textColor);
}

void cTextDrawer::drawTextCentered(const char * msg, int y, int color) const {
	int lenghtInPixels = alfont_text_length(font, msg);
	int half = lenghtInPixels / 2;
	int xPos = (game.m_screenX / 2) - half;
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const char * msg, int x, int y, int boxWidth, int boxHeight, int color) const {
	int lenghtInPixels = alfont_text_length(font, msg);
    int heightInPixels = alfont_text_height(font);

    int halfLengthInPixels = lenghtInPixels / 2;
	int xPos = x + ((boxWidth / 2) - halfLengthInPixels);

	int halfHeightInPixels = heightInPixels / 2;
	int yPos = y + ((boxHeight / 2) - halfHeightInPixels);

    drawText(xPos, yPos, color, msg);
}

void cTextDrawer::drawTextCenteredInBox(const char * msg, const cRectangle & rect, int color) const {
    drawTextCenteredInBox(msg, rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCenteredInBox(const char * msg, const cRectangle & rect, int color, int offsetX, int offsetY) const {
    drawTextCenteredInBox(msg, rect.getX() + offsetX, rect.getY() + offsetY, rect.getWidth(), rect.getHeight(), color);
}

void cTextDrawer::drawTextCentered(const char * msg, int x, int width, int y, int color) const {
	int lenghtInPixels = alfont_text_length(font, msg);
	int half = lenghtInPixels / 2;
	int xPos = x + ((width / 2) - half);
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextBottomRight(const char * msg) const {
    drawTextBottomRight(textColor, msg);
}

void cTextDrawer::drawTextBottomLeft(const char * msg) const {
    drawTextBottomLeft(textColor, msg);
}

void cTextDrawer::drawTextBottomRight(int color, const char * msg) const {
	int lenghtInPixels = alfont_text_length(font, msg);
	int x = game.m_screenX - lenghtInPixels;
	int y = game.m_screenY - getFontHeight();
	drawText(x, y, color, msg);
}

int cTextDrawer::getFontHeight() const {
    assert(font && "Font not set!?");
    return alfont_text_height(font);
}

void cTextDrawer::drawTextBottomLeft(int color, const char * msg) const {
	int y = game.m_screenY - alfont_text_height(font);
	drawText(0, y, color, msg);
}

int cTextDrawer::textLength(const char * msg) const {
    return alfont_text_length(font, msg);
}

void cTextDrawer::setFont(ALFONT_FONT *theFont) {
    if (theFont == nullptr) return; // do not allow null
    this->font = theFont;
}

cRectangle cTextDrawer::getRect(int x, int y, const char * msg) const {
    return cRectangle(x, y, textLength(msg), getFontHeight());
}

cRectangle *cTextDrawer::getAsRectangle(int x, int y, const char * msg) const {
    return new cRectangle(x, y, textLength(msg), getFontHeight());
}

void cTextDrawer::drawText(int x, int y, const char *msg, const char *var) const {
    drawText(x, y, textColor, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, const char *msg, const char *var) const {
    if (applyShadow) {
        alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var);
    }
    alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, const char *msg, int var) const {
    drawTextWithOneInteger(x, y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, const char *msg, int var1, int var2) const {
    drawTextWithTwoIntegers(x, y, msg, var1, var2);
}

void cTextDrawer::drawText(int x, int y, const char *msg, int var) const {
    drawText(x, y, textColor, msg, var);
}
