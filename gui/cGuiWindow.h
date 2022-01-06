#ifndef D2TM_CGUIWINDOW_H
#define D2TM_CGUIWINDOW_H

#include <vector>
#include "cGuiObject.h"
#include "../observers/cInputObserver.h"

class cGuiWindow : cGuiObject {
public:
    cGuiWindow(cRectangle rect);
    ~cGuiWindow();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void draw() const override;

    void addGuiObject(cGuiObject *guiObject);

    cRectangle getRelativeRect(int x, int y, int width, int height);

    void setTitle(const std::string &value) { title = value; }

private:
    std::vector<cGuiObject *> gui_objects;
    std::string title;
    cTextDrawer textDrawer;
};


#endif //D2TM_CGUIWINDOW_H
