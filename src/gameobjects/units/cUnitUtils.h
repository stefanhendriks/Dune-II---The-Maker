/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#pragma once

class cUnitUtils {
  protected:
  	cUnitUtils();

  public:
	  static cUnitUtils& getInstance();

	  int findUnit(int type, int iPlayerId); // find the first unit of type belonging to player Id.
	  int findUnit(int type, int iPlayerId, int iIgnoreUnitId);
};
