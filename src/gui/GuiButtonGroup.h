#include <vector>

class GuiStateButton; 

class GuiButtonGroup {
public:
    GuiButtonGroup() = default;
    void add(GuiStateButton* button);
    void updateStates(GuiStateButton* clickedButton);
    void updateState(int index);
    bool isCurrentlySelected(const GuiStateButton* button) const;
private:
    std::vector<GuiStateButton*> members;
    GuiStateButton* m_currentlySelected = nullptr;
};