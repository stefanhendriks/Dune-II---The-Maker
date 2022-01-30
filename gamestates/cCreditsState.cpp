#include "d2tmh.h"
#include "cCreditsState.h"

#include <alfont.h>
#include <allegro.h>

cCreditsState::cCreditsState(cGame &theGame) : cGameState(theGame), textDrawer(cTextDrawer(bene_font)) {
    moveSpeed = 0.25f;

    bmp_D2TM_Title = (BITMAP *) gfxinter[BMP_D2TM].dat;
    bmp_Dune = (BITMAP *) gfxinter[BMP_GAME_DUNE].dat;

    int duneAtTheRight = game.m_screenX - (bmp_Dune->w * 1.1f);
    int duneAlmostAtBottom = game.m_screenY - (bmp_Dune->h * 1.1f);
    coords_Dune = cPoint(duneAtTheRight, duneAlmostAtBottom);

    int logoWidth = bmp_D2TM_Title->w;
    logoHeight = bmp_D2TM_Title->h;

    int centerOfScreen = game.m_screenX / 2;

    logoX = centerOfScreen - (logoWidth / 2);
    resetCrawler();

    lines = std::vector<s_CreditLine>();
    prepareCrawlerLines();
}

void cCreditsState::prepareCrawlerLines() {
    int colorYellow = makecol(255, 207, 41);
    int colorWhite = makecol(255, 255, 255);
    int fontHeightWithALittlePadding = textDrawer.getFontHeight() + 4;
    int spacerAfterHeadline = fontHeightWithALittlePadding + textDrawer.getFontHeight();
    auto spacer = s_CreditLine {
        .txt = "",
        .color = makecol(0,0,0),
        .height = 40
    };

    lines.push_back(s_CreditLine {
        .txt = fmt::format("Version {}", game.m_version),
        .color = colorYellow,
        .height = spacerAfterHeadline
    });
    lines.push_back(s_CreditLine {
            .txt = "by Stefan Hendriks",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(spacer);
    lines.push_back(s_CreditLine {
        .txt = "a remake of",
        .color = colorWhite,
        .height = spacerAfterHeadline
    });
    lines.push_back(s_CreditLine {
        .txt = "Dune 2 - The Building of a Dynasty (by Westwood Studios)",
        .color = makecol(255, 0, 0),
        .height = spacerAfterHeadline
    });

    lines.push_back(spacer);

    lines.push_back(s_CreditLine {
            .txt = "--- CONTRIBUTORS ---",
            .color = colorWhite,
            .height = spacerAfterHeadline,
    });
    lines.push_back(s_CreditLine {
            .txt = "Paul - Developer",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Arjen - Developer",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Rozmy - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Nema Fakei - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Timenn - Graphics",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Ripps - Testing & Skirmish Maps",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Bruno Antunes ",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });

    lines.push_back(spacer);

    lines.push_back(s_CreditLine {
            .txt = "--- ACKNOWLEDGEMENTS ---",
            .color = colorWhite,
            .height = spacerAfterHeadline
    });
    lines.push_back(s_CreditLine {
            .txt = "Westwood Studios - for producing Dune 2",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Allegro - by Shawn Hargreaves (et al)",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "AlFONT - by Javier Gonzalez",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "...",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });

    lines.push_back(spacer);

    lines.push_back(s_CreditLine {
            .txt = "--- KO-FI SUPPORTERS ---",
            .color = colorWhite,
            .height = spacerAfterHeadline
    });
    lines.push_back(s_CreditLine {
            .txt = "Fedaygin",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "Morto",
            .color = colorWhite,
            .height = fontHeightWithALittlePadding
    });
    lines.push_back(s_CreditLine {
            .txt = "The General",
            .color = colorWhite,
            .height = spacerAfterHeadline
    });
    lines.push_back(spacer);
    lines.push_back(s_CreditLine {
            .txt = "https://ko-fi.com/dune2themaker",
            .color = colorWhite,
            .height = spacerAfterHeadline
    });
}

void cCreditsState::resetCrawler() {
    crawlerY = game.m_screenY + 1;
}

cCreditsState::~cCreditsState() {
    bmp_Dune = nullptr;
    bmp_D2TM_Title = nullptr;
}

void cCreditsState::thinkFast() {
    crawlerY -= moveSpeed;

    int textCrawlY = crawlerY + logoHeight;
    for (auto & line : lines) {
        textCrawlY += line.height;
    }

    if (textCrawlY < 0) {
        resetCrawler();
    }
}

void cCreditsState::draw() const {
    clear_to_color(bmp_screen, makecol(0, 0, 16));

    draw_sprite(bmp_screen, bmp_Dune, coords_Dune.x, coords_Dune.y);

    // draw crawler
    draw_sprite(bmp_screen, bmp_D2TM_Title, logoX, crawlerY);
    int textCrawlY = crawlerY + logoHeight;
    for (auto & line : lines) {
        textDrawer.drawTextCentered(line.txt.c_str(), textCrawlY, line.color);
        textCrawlY += line.height;
    }

    // MOUSE
    game.getMouse()->draw();
}

void cCreditsState::onNotifyMouseEvent(const s_MouseEvent &event) {
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED) {
        moveSpeed = 1.0f; // speed up
    } else if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED) {
        moveSpeed = 0.25f; // restore
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
