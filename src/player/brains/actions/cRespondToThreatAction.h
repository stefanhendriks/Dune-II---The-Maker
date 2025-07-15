#pragma once

class cUnit;
class cPlayer;

/**
 * A simple approach instead of having a function lurking somewhere. We might even benefit from having 'actions' to
 * re-use in AI's. Especially since the Campaign and Skirmish AI's have a lot of overlap.
 *
 * However, by then we might want to have an abstraction for "actions" as well. However, lets not over-engineer.
 *
 * Keep it simple for now.
 *
 * --------
 *
 * This class will make a player respond to a certain 'threat'. It will do that by retaliating with an amount of
 * units (given by 'maxUnitsToOrder' constructor argument). It will take into consideration if a victim unit was
 * passed in. Meaning, when that is a non-attacking unit (ie a Harvester), it will try to run over infantry units (if
 * they were a threat) - or it will flee.
 *
 * When air units attack, only retaliate with anti-air units.
 */
class cRespondToThreatAction {

public:
    cRespondToThreatAction(cPlayer *player, cUnit *threat, cUnit *victim, int cellOriginOfThreat, int maxUnitsToOrder);

    void execute();

private:
    // player responding to threat
    cPlayer *m_player;

    // Cause of threat (currently only a unit can be a threat)
    // pointer because it can be nullptr! (ie a structure can be a threat)
    cUnit *m_threat;
    int m_cellOriginOfThreat;

    // the unit that was damaged by threat
    cUnit *m_victim;

    // the amount of units to use to retaliate
    int m_maxUnitsToOrder;

};