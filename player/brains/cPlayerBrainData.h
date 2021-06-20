#ifndef D2TM_CPLAYERBRAINDATA_H
#define D2TM_CPLAYERBRAINDATA_H

namespace brains {

    enum ePlayerBrainState {

        PLAYERBRAIN_PEACEFUL,

        PLAYERBRAIN_ENEMY_DETECTED,

        PLAYERBRAIN_LOSING,

        PLAYERBRAIN_WINNING

    };

    /**
     * Initially AI is peaceful as it has not discovered any enemies yet
     */
    enum ePlayerBrainCampaignThinkState {
        /**
         * The AI takes a little rest (does nothing) for a certain amount of time.
         */
        PLAYERBRAIN_CAMPAIGN_STATE_REST,

        /**
         * Scan base, requires any repairs?
         */
        PLAYERBRAIN_CAMPAIGN_STATE_SCAN_BASE,

        /**
         * Scan missions, do we have any? If not, think of some...
         */
        PLAYERBRAIN_CAMPAIGN_STATE_MISSIONS,

        /**
         * Process build orders
         */
        PLAYERBRAIN_CAMPAIGN_STATE_PROCESS_BUILDORDERS,

        /**
         * Evaluate state of affairs so far. Enough money in the bank? Base doing ok?
         * This could result into state ENDGAME.
         */
        PLAYERBRAIN_CAMPAIGN_STATE_EVALUATE,

        /**
         * Player is nearly defeated
         */
        PLAYERBRAIN_CAMPAIGN_STATE_ENDGAME

    };

    struct S_structurePosition {
        int cell;
        int type;
        int structureId;
        bool isDestroyed;
    };


    struct S_buildOrder {
        eBuildType buildType; // ie UNIT
        int priority;       // the higher, the more priority to build this
        int buildId;        // ie QUAD
        int placeAt;        // if buildType = STRUCTURE, this is the cell it should be placed at
        buildOrder::eBuildOrderState state;
//
//    bool operator<(const S_buildOrder &order) const {
//        return (priority < order.priority);
//    }
//
//    bool operator>(const S_buildOrder &order) const {
//        return (priority > order.priority);
//    }
    };

}

#endif //D2TM_CPLAYERBRAINDATA_H
