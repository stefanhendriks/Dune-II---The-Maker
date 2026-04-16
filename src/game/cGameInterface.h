#pragma once

class cGame;

class cGameInterface
{
public:
    cGameInterface(cGame* game);
    virtual ~cGameInterface() = default;

    void prepareMentatToTellAboutHouse(int house) const;

private:
    cGame* m_igame = nullptr;
};