/*

 Dune II - The Maker

 Author : Stefan Hendriks
 Contact: stefanhen83@gmail.com
 Website: http://dune2themaker.fundynamic.com

 2001 - 2012 (c) code by Stefan Hendriks

 */
#pragma once
#ifndef TIMERS_H
#define TIMERS_H
	void timerPerSecondFunction(); /** FPS timer **/
	void timerGameTimerFunction(); /** Game timer **/

	extern volatile int timerPerSecond;
	extern volatile int timerGameTimer;
#endif
