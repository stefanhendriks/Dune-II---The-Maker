#include <vector>

class GuiStateButton; 

class GuiButtonGroup {
public:
    GuiButtonGroup() = default;
    void add(GuiStateButton* button);
    void updateStates(GuiStateButton* clickedButton);
    void updateState(int index);
private:
    std::vector<GuiStateButton*> members;
    GuiStateButton* m_currentlySelected = nullptr;
};