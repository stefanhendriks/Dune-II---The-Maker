/**
 * @file cMousePlaceState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

class cKeyboardEvent;
class AudioContext;
class cMouse;
#include "utils/audioID.h"
#include "cMouseState.h"
#include "sGameEvent.h"
class cBuildingListItem;
struct s_MouseEvent;

class cGameControlsContext;
class cPlayer;

/**
 * Mouse placing structure at battlefield
 */
class cMousePlaceState : public cMouseState {

public:
    explicit cMousePlaceState(cPlayer *player, cGameControlsContext *context, cMouse *mouse);

    // looks up the sounds this state plays; called once the audio context is available
    void resolveSounds(const AudioContext *audioContext);

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;
    void onNotifyGameEvent(const s_GameEvent &) override {}

    void onStateSet() override;

    void onFocus() override;
    void onBlur() override;

private:
    audioID m_placeSound = noAudioID;

    void onMouseLeftButtonClicked();

    void onMouseRightButtonPressed();

    void onMouseRightButtonClicked();

    void onMouseMovedTo();

    bool mayPlaceIt(cBuildingListItem *itemToPlace, int mouseCell);
};
