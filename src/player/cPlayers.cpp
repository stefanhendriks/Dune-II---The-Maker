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
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_PLAYER, "cPlayers::operator[]", 
            std::format("Invalid player index: {}", index));
        return m_players[0];  // Return first player as fallback
    }
    return m_players[index];
}

const cPlayer& cPlayers::operator[](int index) const {
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        cLogger::getInstance()->log(LOG_ERROR, eLogComponent::COMP_PLAYER, "cPlayers::operator[] const", 
            std::format("Invalid player index: {}", index));
        return m_players[0];  // Return first player as fallback
    }
    return m_players[index];
}

cPlayer* cPlayers::getPlayer(int index) {
    if (index < 0 || index >= MAX_PLAYERS_CAPACITY) {
        return nullptr;
    }
    return &m_players[index];
}

const cPlayer* cPlayers::getPlayer(int index) const {
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
