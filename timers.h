/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */
#pragma once

void allegro_timerseconds();	/** FPS timer **/
void allegro_timergametime(); /** Global timer **/
void allegro_timerunits();	/** Unit timer **/

extern volatile int allegro_timerSecond;
extern volatile int allegro_timerGlobal;
extern volatile int allegro_timerUnits;
