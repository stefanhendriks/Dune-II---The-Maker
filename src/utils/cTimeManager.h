/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

/**
	Time management is done in this class
*/

#pragma once
#include <stdint.h>
class cGame;

class cTimeManager {

private:

    int m_timerUnits;		/** !!Specificly!! used for units **/
    int m_timerSecond;
    int m_timerGlobal;

    uint64_t m_lastUnitsTick = 0;
    uint64_t m_lastGameTimeTick = 0;
    uint64_t m_lastSecondsTick = 0;

    void handleTimerUnits();
    void handleTimerAllegroTimerSeconds();
    void handleTimerGameTime();

    void capTimers();

    /** Allegro specific routines to handle timers **/
    // void syncFromAllegroTimers();
    // void syncAllegroTimers();

public:

    explicit cTimeManager(cGame &game);

    cGame &m_game;
    int m_gameTime;		/** Definition of game time (= in seconds) **/

    void processTime();
};
