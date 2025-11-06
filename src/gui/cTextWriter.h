/*
 * cTextWriter.h
 *
 *  Created on: 1-nov-2010
 *      Author: Stefan
 */

#pragma once

#include "drawers/cTextDrawer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <format>

// a text writer has state, meaning with every command to 'write' something, it knows where to draw it
// every new command it will start on a new line.
class cTextWriter {
public:
    cTextWriter(cTextDrawer* _textDrawer,int theFontSize);
    ~cTextWriter();

    void write(const std::string &msg);
    void write(const std::string &msg, Color color);

    template<typename... Args>
    void writef(std::format_string<Args...> fmtStr, Args &&... args) {
        write(std::format(fmtStr, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void writef(Color color, std::format_string<Args...> fmtStr, Args &&... args) {
        write(std::format(fmtStr, std::forward<Args>(args)...), color);
    }

    void moveTo(int x, int y);

protected:
    void updateDrawY();

private:
    cTextDrawer *textDrawer;
    int fontSize;
    int drawY;
    int drawX;
};
