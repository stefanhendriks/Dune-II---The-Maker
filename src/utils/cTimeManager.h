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
public:
    explicit cTimeManager(cGame *game);
    void processTime();

    int getFps() const;
    int getWaitingTime() const { return waitingTime; }
    void waitForCPU();
    void capFps();
    void adaptWaitingTime();

private:
    // gametime timer is called every 100 ms, try to keep up with that.
    void handleTimerUnits();
    // gametime timer is called every 1000 ms, try to keep up with that.
    void handleTimerSecond();
    // gametime timer is called every 5 ms, try to keep up with that.
    void handleTimerGameTime();
  	// system capping to avoid extremely high timers
    void capTimers();

    cGame *m_game;
    int m_timerUnits;		/** !!Specificly!! used for units **/
    int m_timerSecond;
    int m_timerGlobal;
    int m_gameTime;		/** Definition of game time (= in seconds) **/

    int m_fps = 0;			/** Frames per second **/
    int frameCount = 0;		/** Frame count for FPS calculation **/
    int waitingTime = 10;	/** Waiting time in ms, used to adapt FPS **/
    uint64_t m_lastUnitsTick = 0;
    uint64_t m_lastGameTimeTick = 0;
    uint64_t m_lastSecondsTick = 0;


};
