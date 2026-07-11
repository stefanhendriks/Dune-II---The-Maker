#pragma once

// Pure predicate used by cPlayer::hasAliveUnit().
// Extracted here so it can be tested without pulling in the full cUnit dependency chain.
//
// A unit counts as alive for defeat evaluation when it is either:
//   - not dead (iHitPoints > 0), or
//   - carried (iHitPoints == -1 with iTempHitPoints > 0, meaning it is inside a carryall
//     or a structure; it will return to the map later).
//
// isCarried: reflects cUnit::isCarried() / cUnit::isHidden(), i.e. iTempHitPoints > -1.
// isDead:    reflects cUnit::isDead(),                         i.e. iHitPoints <= 0.
inline bool unitCountsAsAliveForDefeat(bool isDead, bool isCarried) {
    return !isDead || isCarried;
}
