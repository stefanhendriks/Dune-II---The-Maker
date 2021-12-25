#include "d2tmh.h"

#include "cMouseNormalState.h"

cMouseNormalState::cMouseNormalState(cGameControlsContext *context, cMouse *mouse) : cMouseState(context, mouse) {

}

void cMouseNormalState::onNotifyMouseEvent(const s_MouseEvent &event) {

    switch (event.eventType) {
        case MOUSE_LEFT_BUTTON_PRESSED:
            mouse->boxSelectLogic(context->getMouseCell());
            break;
        case MOUSE_LEFT_BUTTON_CLICKED:
            onMouseLeftButtonClicked(event);
        default:
            break;
    }

    mouse->setTile(mouseTile);
}

cMouseNormalState::~cMouseNormalState() {

}

void cMouseNormalState::onMouseLeftButtonClicked(const s_MouseEvent &event) {
// where we box selecting? then this must be the unpress of the mouse button and thus we
//        // should start selecting units within the rectangle
//        if (mouse->isBoxSelecting()) {
//            cRectangle boxSelectRectangle = mouse->getBoxSelectRectangle();
//
//            //  char msg[256];
//            //  sprintf(msg, "MINX=%d, MAXX=%d, MINY=%d, MAXY=%d", min_x, min_y, max_x, max_y);
//            //  logbook(msg);
//
//            // Now do it!
//            // deselect all units
//            UNIT_deselect_all();
//            mouse->setTile(MOUSE_NORMAL);
//
//            bool bPlayRep = false;
//            bool bPlayInf = false;
//
//            bool harvesterSelected=false;
//            bool attackingUnitSelected=false;
//
//            for (int i = 0; i < MAX_UNITS; i++) {
//                cUnit &cUnit = unit[i];
//                if (!cUnit.isValid()) continue;
//                if (cUnit.iPlayer != HUMAN) continue;
//                // do not select airborn units
//                if (cUnit.isAirbornUnit()) {
//                    // always deselect unit:
//                    cUnit.bSelected = false;
//                    continue;
//                }
//                if (cUnit.iTempHitPoints >= 0) continue; // skip units that have been 'hidden' in a refinery or repair
//
//                if (boxSelectRectangle.isPointWithin(cUnit.center_draw_x(), cUnit.center_draw_y())) {
//                    // It is in the borders, select it
//                    cUnit.bSelected = true;
//
//                    if (cUnit.iType == HARVESTER) {
//                        harvesterSelected = true;
//                    } else {
//                        attackingUnitSelected = true;
//                    }
//
//                    if (sUnitInfo[cUnit.iType].infantry) {
//                        bPlayInf = true;
//                    } else {
//                        bPlayRep = true;
//                    }
//
//                }
//            }
//
//            if (harvesterSelected && attackingUnitSelected) {
//                // unselect harvesters
//                for (int i = 0; i < MAX_UNITS; i++) {
//                    cUnit &cUnit = unit[i];
//                    if (!cUnit.isValid()) continue;
//                    if (cUnit.iPlayer != HUMAN) continue;
//                    if (!cUnit.bSelected) continue;
//                    if (cUnit.iType == HARVESTER) {
//                        cUnit.bSelected = false; // unselect
//                    }
//                }
//            }
//
//            if (bPlayInf || bPlayRep) {
//                if (bPlayRep)
//                    play_sound_id(SOUND_REPORTING);
//
//                if (bPlayInf)
//                    play_sound_id(SOUND_YESSIR);
//
//                bOrderingUnits = true;
//            }
//
//        }
//
    mouse->resetBoxSelect();
}
