#include <catch2/catch_test_macros.hpp>

#include "game/GameStateMachine.hpp"
#include "FakeStateCreator.hpp"
#include "NullStateCallbacks.hpp"

namespace {
struct Fixture {
    FakeStateCreator creator;
    NullStateCallbacks callbacks;
    GameStateMachine machine{creator, callbacks};
};
}

TEST_CASE("GameStateMachine starts in INITIALIZE without a current state", "[gamestate]") {
    Fixture f;
    CHECK(f.machine.getState() == eGameState::INITIALIZE);
    CHECK(f.machine.getCurrentState() == nullptr);
}

TEST_CASE("GameStateMachine transition from INITIALIZE to CREDITS", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::CREDITS);
    CHECK(f.machine.getState() == eGameState::CREDITS);
    CHECK(f.machine.getCurrentState() != nullptr);
    CHECK(f.machine.getCurrentState() == f.creator.getState(eGameState::CREDITS));
}

TEST_CASE("GameStateMachine transition to INITIALIZE creates no state", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::CREDITS);
    f.machine.transitionTo(eGameState::INITIALIZE);
    CHECK(f.machine.getState() == eGameState::INITIALIZE);
    CHECK(f.creator.m_created.size() == 1);
}

TEST_CASE("GameStateMachine transition to the same state is a no-op", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::CREDITS);
    auto *current = f.machine.getCurrentState();
    f.callbacks.m_calls.clear();
    f.creator.m_created.clear();
    f.creator.m_destroyed.clear();

    f.machine.transitionTo(eGameState::CREDITS);

    CHECK(f.machine.getState() == eGameState::CREDITS);
    CHECK(f.machine.getCurrentState() == current);
    CHECK(f.callbacks.m_calls.empty());
    CHECK(f.creator.m_created.empty());
    CHECK(f.creator.m_destroyed.empty());
}

TEST_CASE("GameStateMachine destroys OPTIONS when going to PLAYING", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::PLAYING);
    f.machine.transitionTo(eGameState::OPTIONS);
    REQUIRE(f.creator.hasState(eGameState::OPTIONS));

    f.machine.transitionTo(eGameState::PLAYING);

    CHECK_FALSE(f.creator.hasState(eGameState::OPTIONS));
    CHECK(f.machine.getState() == eGameState::PLAYING);
    CHECK(f.machine.getCurrentState() == f.creator.getState(eGameState::PLAYING));
    CHECK(f.callbacks.called("playing:fromOptions"));
}

TEST_CASE("GameStateMachine destroys OPTIONS when going to REGION", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::OPTIONS);
    REQUIRE(f.creator.hasState(eGameState::OPTIONS));

    f.machine.transitionTo(eGameState::REGION);

    CHECK_FALSE(f.creator.hasState(eGameState::OPTIONS));
    CHECK(f.machine.getCurrentState() == f.creator.getState(eGameState::REGION));
}

TEST_CASE("GameStateMachine reuses the PLAYING state", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::PLAYING);
    auto *playing = f.machine.getCurrentState();

    f.machine.transitionTo(eGameState::OPTIONS);
    f.machine.transitionTo(eGameState::PLAYING);
    CHECK(f.machine.getCurrentState() == playing);

    f.machine.transitionTo(eGameState::MENU);
    f.machine.transitionTo(eGameState::PLAYING);
    CHECK(f.machine.getCurrentState() == playing);
    CHECK(f.callbacks.called("playing:new"));
    CHECK(f.callbacks.called("playing:existing"));
}

TEST_CASE("GameStateMachine always recreates MISSIONSELECT", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::MISSIONSELECT);
    auto *first = f.machine.getCurrentState();
    f.machine.transitionTo(eGameState::MENU);
    f.machine.transitionTo(eGameState::MISSIONSELECT);

    CHECK(f.machine.getCurrentState() != first);
}

TEST_CASE("GameStateMachine keeps the pre-created VIDEO state", "[gamestate]") {
    Fixture f;
    auto *video = f.creator.createVideoState();
    f.creator.m_destroyed.clear();

    f.machine.transitionTo(eGameState::VIDEO);

    CHECK(f.machine.getCurrentState() == video);
    CHECK(f.creator.m_destroyed.empty());
}

TEST_CASE("GameStateMachine drops the conquest state when selecting a house", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::REGION);
    auto *region = f.machine.getCurrentState();

    f.machine.transitionTo(eGameState::SELECT_HOUSE);
    CHECK_FALSE(f.creator.hasState(eGameState::REGION));

    f.machine.transitionTo(eGameState::REGION);
    CHECK(f.machine.getCurrentState() != region);
    CHECK(f.callbacks.called("region:new"));
}

TEST_CASE("GameStateMachine REGION coming from a brief sets up the next mission", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::REGION);
    f.machine.transitionTo(eGameState::WINBRIEF);
    f.callbacks.m_calls.clear();

    f.machine.transitionTo(eGameState::REGION);

    CHECK(f.callbacks.called("region:existing:fromBrief"));
    CHECK(f.callbacks.called("music:conquest"));
}

TEST_CASE("GameStateMachine OPTIONS remembers where it came from", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::MENU);
    f.machine.transitionTo(eGameState::OPTIONS);
    CHECK(f.callbacks.called("options:new"));
    f.machine.transitionTo(eGameState::MENU);

    f.machine.transitionTo(eGameState::OPTIONS);

    CHECK(f.callbacks.called("options:existing"));
    CHECK(f.callbacks.m_optionsExistingFrom == eGameState::MENU);
}

TEST_CASE("GameStateMachine side effects of entering the menu", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::MENU);
    CHECK(f.callbacks.called("cheat:off"));
    CHECK(f.callbacks.called("music:menu"));
}

TEST_CASE("GameStateMachine prepares before creating the state", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::TELLHOUSE);
    f.machine.transitionTo(eGameState::SETUPSKIRMISH);
    CHECK(f.callbacks.called("tellHouse"));
    CHECK(f.callbacks.called("skirmish"));
}

TEST_CASE("GameStateMachine lets the callbacks handle the LOSEBRIEF music", "[gamestate]") {
    Fixture f;
    f.machine.transitionTo(eGameState::LOSEBRIEF);
    CHECK(f.callbacks.called("loseBrief"));
    CHECK_FALSE(f.callbacks.called("music:briefing"));
}
