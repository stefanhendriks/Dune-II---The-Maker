#include "gui/GuiObject.h"
#include "utils/cEnumArray.h"

#include <memory>
#include <functional>

class Texture;

struct GuiStateButtonParams {
    cRectangle rect;
    GuiRenderKind kind = GuiRenderKind::TRANSPARENT_WITHOUT_BORDER;
    GuiTheme theme = GuiTheme::Light();
    std::function<void()> onLeftClick = nullptr;
    std::function<void()> onRightClick = nullptr;
    Texture *tex = nullptr;
};

class GuiStateButton : public GuiObject {
public:
    explicit GuiStateButton(const cRectangle &rect);
    ~GuiStateButton();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void setTexture(Texture* tex);
    void setRenderKind(GuiRenderKind value);
    void draw() const override;
    void changeState();
    void setOnLeftMouseButtonClickedAction(std::function<void()> action);
    void setOnRightMouseButtonClickedAction(std::function<void()> action);
private:
    enum class GuiState : char {DISABLED =0, CLICKED, UNCLICKED, COUNT};
    Texture* m_tex =nullptr;
    EnumArray<std::unique_ptr<cRectangle>,GuiState> rectState;
    GuiRenderKind m_renderKind;
    GuiState m_state;
    cRectangle* m_currentRectState;
    std::function<void()> m_onLeftMouseButtonClickedAction;
    std::function<void()> m_onRightMouseButtonClickedAction;
};



class GuiStateButtonBuilder {
public:
    GuiStateButtonBuilder& withRect(const cRectangle& rect) {
        params.rect = rect;
        return *this;
    }

    GuiStateButtonBuilder& withKind(GuiRenderKind kind) {
        params.kind = kind;
        return *this;
    }

    GuiStateButtonBuilder& onClick(std::function<void()> callback) {
        params.onLeftClick = std::move(callback);
        return *this;
    }

    GuiStateButtonBuilder& onRightClick(std::function<void()> callback) {
        params.onRightClick = std::move(callback);
        return *this;
    }

    GuiStateButtonBuilder& withTexture(Texture* tex) {
        params.tex = tex;
        return *this;
    }

    GuiStateButton* build() const {
        GuiStateButton* btn = new GuiStateButton(params.rect);
        btn->setRenderKind(params.kind);
        btn->setTheme(params.theme);
        btn->setTexture(params.tex);
        if (params.onLeftClick) {
            btn->setOnLeftMouseButtonClickedAction(params.onLeftClick);
        }
        if (params.onRightClick) {
            btn->setOnRightMouseButtonClickedAction(params.onRightClick);
        }        
        return btn;
    }

private:
    GuiStateButtonParams params;
};
