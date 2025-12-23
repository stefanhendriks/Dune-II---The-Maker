#include <vector>

class GuiStateButton; 

class GuiButtonGroup {
public:
    GuiButtonGroup() = default;
    void add(GuiStateButton* button) {
        members.push_back(button);
    }
    void updateStates(GuiStateButton* clickedButton);
private:
    std::vector<GuiStateButton*> members;
};