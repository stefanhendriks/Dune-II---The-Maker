#include "cCreditsState.h"

#include "d2tmc.h"
#include "data/gfxinter.h"
#include "drawers/cAllegroDrawer.h"

// #include <alfont.h>
#include <allegro.h>
#include <SDL2/SDL.h>

cCreditsState::cCreditsState(cGame &theGame) :
    cGameState(theGame),
    m_moveSpeed(0.15f),
    m_textDrawer(cTextDrawer(bene_font)),
    m_duneBmp((BITMAP *) gfxinter[BMP_GAME_DUNE].dat),
    m_titleBmp((BITMAP *) gfxinter[BMP_D2TM].dat)
{

    int duneAtTheRight = game.m_screenW - (m_duneBmp->w * 1.1f);
    int duneAlmostAtBottom = game.m_screenH - (m_duneBmp->h * 1.1f);
    m_duneCoordinates = cPoint(duneAtTheRight, duneAlmostAtBottom);

    int titleWidth = m_titleBmp->w;
    m_titleHeight = m_titleBmp->h;

    int centerOfScreen = game.m_screenW / 2;

    m_titleX = centerOfScreen - (titleWidth / 2);
    resetCrawler();

    m_lines = std::vector<s_CreditLine>();
    prepareCrawlerLines();
}

void cCreditsState::prepareCrawlerLines()
{
    SDL_Color colorYellow = SDL_Color{255, 207, 41,255};
    SDL_Color colorWhite = SDL_Color{255, 255, 255,255};
    SDL_Color colorGreen = SDL_Color{0, 255, 0,255};
    SDL_Color colorRed = SDL_Color{255, 0, 0,255};

    SDL_Color colorHeadlines = colorGreen;

    int fontHeightWithALittlePadding = m_textDrawer.getFontHeight() + 4;
    int spacerAfterHeadline = fontHeightWithALittlePadding + m_textDrawer.getFontHeight();
    auto spacer = s_CreditLine {
        .name = "",
        .txt = "",
        .color = SDL_Color{0,0,0,255},
        .height = 40
    };

    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = fmt::format("Version {}", game.m_version),
        .color = colorYellow,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "by Stefan Hendriks",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(spacer);
    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "a remake of",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "Dune 2 - The Building of a Dynasty (by Westwood Studios)",
        .color = colorRed,
        .height = spacerAfterHeadline
    });

    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "--- CONTRIBUTORS ---",
        .color = colorHeadlines,
        .height = spacerAfterHeadline,
    });
    m_lines.push_back(s_CreditLine {
        .name = "Paul",
        .txt = "Developer",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Arjen",
        .txt = "Developer & Sparring partner",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Rozmy",
        .txt = "Graphics",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Nema Fakei",
        .txt = "Graphics",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Vidiware",
        .txt = "Graphics",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Timenn",
        .txt = "Graphics",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "MrFlibble",
        .txt = "Graphics",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Ripps",
        .txt = "Support at Discord, testing & Skirmish Maps",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Bruno",
        .txt = "For your lifelong support & valuable feedback",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });

    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "--- ACKNOWLEDGEMENTS ---",
        .color = colorHeadlines,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Westwood Studios",
        .txt = "for producing Dune 2",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Allegro",
        .txt = "by Shawn Hargreaves (et al)",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "AlFONT",
        .txt = "by Javier Gonzalez",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "Dune Legacy, Dune Dynasty and Dune The Golden Path",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });

    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "--- KO-FI SUPPORTERS ---",
        .color = colorHeadlines,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Fedaygin",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Morto",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "The General",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Bastian",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Florian",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Aleksandar",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Bruno",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Dan Greene",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Sergio Muad'Dib",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "saif3r",
        .txt = "",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "--- SPECIAL THANKS TO ---",
        .color = colorHeadlines,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .name = "Gobalopper",
        .txt = "for Dune2K, and hosting arrakis.dune2k.com",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Jpexs",
        .txt = "for help with the Dune 2 Seed Generator",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Rene",
        .txt = "for the D2TM Java Map Editor",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Guillaume Lavoie",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Dennis Stansert",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Ash",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Ruurd Nijdam",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "rphl",
        .txt = "",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Non sono Mandrake",
        .txt = "for nudging me to work on D2TM again in C++",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
        .name = "Dune2k.com",
        .txt = "For all the good times at the arrakis forums",
        .color = colorWhite,
        .height = fontHeightWithALittlePadding
    });

    // spacer
    // and finally a thank you for the player
    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
        .name = "",
        .txt = "Thank you for playing",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
}

void cCreditsState::resetCrawler()
{
    m_crawlerY = game.m_screenH + 1;
}

cCreditsState::~cCreditsState()
{
    m_duneBmp = nullptr;
    m_titleBmp = nullptr;
}

void cCreditsState::thinkFast()
{
    m_crawlerY -= m_moveSpeed;

    int textCrawlY = m_crawlerY + m_titleHeight;
    for (auto &line : m_lines) {
        textCrawlY += line.height;
    }

    if (textCrawlY < 0) {
        resetCrawler();
    }
}

void cCreditsState::draw() const
{
    clear_to_color(bmp_screen, makecol(0, 0, 16));

    renderDrawer->drawSprite(bmp_screen, m_duneBmp, m_duneCoordinates.x, m_duneCoordinates.y);

    int halfScreen = game.m_screenW / 2;

    // draw crawler
    renderDrawer->drawSprite(bmp_screen, m_titleBmp, m_titleX, m_crawlerY);
    int textCrawlY = m_crawlerY + m_titleHeight;
    for (auto &line : m_lines) {
        if (line.name.empty()) {
            m_textDrawer.drawTextCentered(line.txt.c_str(), textCrawlY, line.color);
        }
        else if (line.txt.empty()) {
            m_textDrawer.drawTextCentered(line.name.c_str(), textCrawlY, line.color);
        }
        else {
            int spaceBetween = 4;
            // both name and line given, then we render them differently.
            m_textDrawer.drawText(halfScreen+spaceBetween, textCrawlY, line.txt.c_str());
            int widthOfName = m_textDrawer.textLength(line.name.c_str());
            m_textDrawer.drawText(halfScreen - (widthOfName + spaceBetween), textCrawlY, line.name.c_str());
        }
        textCrawlY += line.height;
    }

    // MOUSE
    game.getMouse()->draw();
}

void cCreditsState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED) {
        m_moveSpeed = 1.0f; // speed up
    }
    else if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED) {
        m_moveSpeed = 0.15f; // restore
    }
}

eGameStateType cCreditsState::getType()
{
    return GAMESTATE_CREDITS;
}

void cCreditsState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(KEY_ESC)) {
        game.setNextStateToTransitionTo(GAME_MENU);
        game.initiateFadingOut();
    }
}
