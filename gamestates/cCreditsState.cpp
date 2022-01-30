#include "d2tmh.h"
#include "cCreditsState.h"

#include <alfont.h>
#include <allegro.h>

cCreditsState::cCreditsState(cGame &theGame) :
    cGameState(theGame),
    m_moveSpeed(0.15f),
    m_textDrawer(cTextDrawer(bene_font)),
    m_duneBmp((BITMAP *) gfxinter[BMP_GAME_DUNE].dat),
    m_titleBmp((BITMAP *) gfxinter[BMP_D2TM].dat) {

    int duneAtTheRight = game.m_screenX - (m_duneBmp->w * 1.1f);
    int duneAlmostAtBottom = game.m_screenY - (m_duneBmp->h * 1.1f);
    m_duneCoordinates = cPoint(duneAtTheRight, duneAlmostAtBottom);

    int titleWidth = m_titleBmp->w;
    m_titleHeight = m_titleBmp->h;

    int centerOfScreen = game.m_screenX / 2;

    m_titleX = centerOfScreen - (titleWidth / 2);
    resetCrawler();

    m_lines = std::vector<s_CreditLine>();
    prepareCrawlerLines();
}

void cCreditsState::prepareCrawlerLines() {
    int colorYellow = makecol(255, 207, 41);
    int colorWhite = makecol(255, 255, 255);
    int colorGreen = makecol(0, 255, 0);
    int colorRed = makecol(255, 0, 0);

    int colorHeadlines = colorGreen;

    int fontHeightWithALittlePadding = m_textDrawer.getFontHeight() + 4;
    int spacerAfterHeadline = fontHeightWithALittlePadding + m_textDrawer.getFontHeight();
    auto spacer = s_CreditLine {
        .txt = "",
        .color = makecol(0,0,0),
        .height = 40
    };

    m_lines.push_back(s_CreditLine {
        .txt = fmt::format("Version {}", game.m_version),
        .color = colorYellow,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
            .txt = "by Stefan Hendriks",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(spacer);
    m_lines.push_back(s_CreditLine {
        .txt = "a remake of",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
        .txt = "Dune 2 - The Building of a Dynasty (by Westwood Studios)",
        .color = colorRed,
        .height = spacerAfterHeadline
    });

    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
            .txt = "--- CONTRIBUTORS ---",
            .color = colorHeadlines,
            .height = spacerAfterHeadline,
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Paul - Developer",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Arjen - Developer & Sparring partner",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Rozmy - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Nema Fakei - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Vidiware - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Timenn - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "MrFlibble - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Ripps - Support at Discord, testing & Skirmish Maps",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Bruno - For your lifelong support & valuable feedback",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });

    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
            .txt = "--- ACKNOWLEDGEMENTS ---",
            .color = colorHeadlines,
            .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Westwood Studios - for producing Dune 2",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Allegro - by Shawn Hargreaves (et al)",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "AlFONT - by Javier Gonzalez",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Dune Legacy, Dune Dynasty and Dune The Golden Path",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });

    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
            .txt = "--- KO-FI SUPPORTERS ---",
            .color = colorHeadlines,
            .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Fedaygin",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Morto",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "The General",
            .color = colorWhite,
            .height = spacerAfterHeadline
    });
    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
            .txt = "--- SPECIAL THANKS TO ---",
            .color = colorHeadlines,
            .height = spacerAfterHeadline
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Gobalopper - for Dune2K, and hosting arrakis.dune2k.com",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Jpexs - for help with the Dune 2 Seed Generator",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Rene - for the D2TM Java Map Editor",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Guillaume Lavoie",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Dennis Stansert",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Ash",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Ruurd Nijdam",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "rphl",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "Non sono Mandrake - for nudging me to work on D2TM again in C++",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    m_lines.push_back(s_CreditLine {
            .txt = "All those at the dune2k arrakis forums and the good times",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });

    // spacer
    // and finally a thank you for the player
    m_lines.push_back(spacer);

    m_lines.push_back(s_CreditLine {
            .txt = "Thank you for playing",
            .color = colorWhite,
            .height = spacerAfterHeadline
    });
}

void cCreditsState::resetCrawler() {
    m_crawlerY = game.m_screenY + 1;
}

cCreditsState::~cCreditsState() {
    m_duneBmp = nullptr;
    m_titleBmp = nullptr;
}

void cCreditsState::thinkFast() {
    m_crawlerY -= m_moveSpeed;

    int textCrawlY = m_crawlerY + m_titleHeight;
    for (auto & line : m_lines) {
        textCrawlY += line.height;
    }

    if (textCrawlY < 0) {
        resetCrawler();
    }
}

void cCreditsState::draw() const {
    clear_to_color(bmp_screen, makecol(0, 0, 16));

    draw_sprite(bmp_screen, m_duneBmp, m_duneCoordinates.x, m_duneCoordinates.y);

    // draw crawler
    draw_sprite(bmp_screen, m_titleBmp, m_titleX, m_crawlerY);
    int textCrawlY = m_crawlerY + m_titleHeight;
    for (auto & line : m_lines) {
        m_textDrawer.drawTextCentered(line.txt.c_str(), textCrawlY, line.color);
        textCrawlY += line.height;
    }

    // MOUSE
    game.getMouse()->draw();
}

void cCreditsState::onNotifyMouseEvent(const s_MouseEvent &event) {
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED) {
        m_moveSpeed = 1.0f; // speed up
    } else if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED) {
        m_moveSpeed = 0.15f; // restore
    }
}

eGameStateType cCreditsState::getType() {
    return GAMESTATE_CREDITS;
}

void cCreditsState::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    if (event.isType(eKeyEventType::PRESSED) && event.hasKey(KEY_ESC)) {
        game.setNextStateToTransitionTo(GAME_MENU);
    }
}
