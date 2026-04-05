/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cPlayer.h"
#include "cPlayers.h"
#include "utils/cLog.h"

cPlayers::cPlayers() {
    // Players will be initialized through default constructors of std::array
}

cPlayer& cPlayers::operator[](int index) {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::operator[] out of bounds");
    return m_players[index];
}

const cPlayer& cPlayers::operator[](int index) const {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::operator[] const out of bounds");
    return m_players[index];
}

cPlayer* cPlayers::getPlayer(int index) {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::getPlayer out of bounds");
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        return nullptr;
    }
    return &m_players[index];
}

const cPlayer* cPlayers::getPlayer(int index) const {
    assert(index >= 0 && index < MAX_PLAYERS_CAPACITY && "cPlayers::getPlayer const out of bounds");
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        return nullptr;
    }
    return &m_players[index];
}

cPlayer& cPlayers::getHumanPlayer() {
    return m_players[0];  // HUMAN is typically player 0
}

const cPlayer& cPlayers::getHumanPlayer() const {
    return m_players[0];  // HUMAN is typically player 0
}
