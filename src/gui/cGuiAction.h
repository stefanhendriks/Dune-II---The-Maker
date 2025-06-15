#pragma once

class cGuiAction {
  public:
    virtual ~cGuiAction() = default;
    virtual void execute() = 0;
};
