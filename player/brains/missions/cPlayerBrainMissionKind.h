#ifndef D2TM_CPLAYERBRAINMISSIONKIND_H
#define D2TM_CPLAYERBRAINMISSIONKIND_H

#include <observers/cScenarioObserver.h>

namespace brains {

    class cPlayerBrainMission;

    enum ePlayerBrainMissionKind {
        /**
         * Attack enemy player (generally)
         */
        PLAYERBRAINMISSION_KIND_ATTACK,

        /**
         * Explore the map
         */
        PLAYERBRAINMISSION_KIND_EXPLORE,

        /**
         * Improve economy (ie, build Carry-All's, harvesters, etc)
         */
        PLAYERBRAINMISSION_IMPROVE_ECONOMY,

        /**
         * Explore the map, find spice patch(es)
         */
        PLAYERBRAINMISSION_KIND_FIND_SPICE,

        /**
         * Attack enemy player (generally) from the air
         */
        PLAYERBRAINMISSION_KIND_AIRSTRIKE,

        /**
         * Defend area?
         */
        PLAYERBRAINMISSION_KIND_DEFEND,

        /**
         * Harass enemy ... (harvesters?)
         */
        PLAYERBRAINMISSION_KIND_HARASS,

        /**
         * Super weapon usage: Fremen
         */
        PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN,

        /**
         * Super weapon usage: Saboteur
         */
        PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR,

        /**
         * Super weapon usage: Deathhand
         */
        PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND,

    };

    static const char* ePlayerBrainMissionKindString(const ePlayerBrainMissionKind &kind) {
        switch (kind) {
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_DEFEND: return "PLAYERBRAINMISSION_KIND_DEFEND";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_ATTACK: return "PLAYERBRAINMISSION_KIND_ATTACK";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_IMPROVE_ECONOMY: return "PLAYERBRAINMISSION_IMPROVE_ECONOMY";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND: return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_DEATHHAND";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR: return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_SABOTEUR";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN: return "PLAYERBRAINMISSION_KIND_SUPERWEAPON_FREMEN";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_EXPLORE: return "PLAYERBRAINMISSION_KIND_EXPLORE";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_AIRSTRIKE: return "PLAYERBRAINMISSION_KIND_AIRSTRIKE";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_FIND_SPICE: return "PLAYERBRAINMISSION_KIND_FIND_SPICE";
            case ePlayerBrainMissionKind::PLAYERBRAINMISSION_KIND_HARASS: return "PLAYERBRAINMISSION_KIND_HARASS";
            default:
                assert(false && "Unknown kind?");
                break;
        }
        return "";
    }

    class cPlayerBrainMissionKind : public cScenarioObserver {

    public:

        cPlayerBrainMissionKind(cPlayer *player, cPlayerBrainMission * mission);

        virtual ~cPlayerBrainMissionKind() = 0;

        virtual bool think_SelectTarget() = 0;

        virtual void think_Execute() = 0;

        virtual cPlayerBrainMissionKind * clone(cPlayer *player, cPlayerBrainMission * mission) = 0;

        virtual void onNotify(const s_GameEvent &event) override = 0;

        virtual const char *toString() = 0;

        void log(const char*txt);

    protected:

        cPlayer *player;
        cPlayerBrainMission *mission;
    };

}

#endif //D2TM_CPLAYERBRAINMISSIONKIND_H
