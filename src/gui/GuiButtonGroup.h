#include <vector>

class GuiStateButton; 

class GuiButtonGroup {
public:
    GuiButtonGroup() = default;
    void add(GuiStateButton* button) {
        members.push_back(button);
    }
    void updateStates(GuiStateButton* clickedButton);
    void updateState(int index);
private:
    std::vector<GuiStateButton*> members;
};