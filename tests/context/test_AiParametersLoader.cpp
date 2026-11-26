#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "context/AiParametersLoader.hpp"
#include "gameobjects/players/brains/sAiParameters.h"

namespace {

std::filesystem::path writeTempFile(const std::string& contents)
{
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = std::filesystem::temp_directory_path() / ("d2tm_test_ai_" + std::to_string(timestamp) + ".ini");
    std::ofstream output(path);
    output << contents;
    return path;
}

} // namespace

TEST_CASE("AiParametersLoader reads values of every kind from their sections")
{
    const auto path = writeTempFile(
        "[SHARED]\nRestTime=20\nRepairStructureBelowHpFraction=0.5\nRespondersWhenEnemyNearBaseBase=3\nRespondersWhenEnemyNearBaseRandom=6\n"
        "[SKIRMISH]\nInitialRestRandom=99\nGroundAttackRichChanceMultiplier=2.5\n"
        "[CAMPAIGN]\nInitialRestTime=100\n"
        "[MISSIONS]\nBuildOrderWaitBuildTimeFactor=1.5\n"
        "[SANDWORM]\nThinkInterval=8\n"
        "[FREMEN_SUPERWEAPON]\nSkipPlayerAbove=60\n");
    brains::s_AiParameters parameters;

    REQUIRE(AiParametersLoader().load(path.string(), parameters));
    std::filesystem::remove(path);

    CHECK(parameters.shared.restTime == 20);
    CHECK(parameters.shared.repairStructureBelowHpFraction == 0.5);
    CHECK(parameters.shared.respondersWhenEnemyNearBase.base == 3);
    CHECK(parameters.shared.respondersWhenEnemyNearBase.randomExtra == 6);
    CHECK(parameters.skirmish.initialRestRandom == 99);
    CHECK(parameters.skirmish.groundAttackRichChanceMultiplier == 2.5);
    CHECK(parameters.campaign.initialRestTime == 100);
    CHECK(parameters.missions.buildOrderWaitBuildTimeFactor == 1.5f);
    CHECK(parameters.sandworm.thinkInterval == 8);
    CHECK(parameters.fremenSuperWeapon.skipPlayerAbove == 60);
}

TEST_CASE("AiParametersLoader keeps the defaults for keys that are missing")
{
    const auto path = writeTempFile("[SHARED]\nRestTime=20\nRespondersWhenHarvesterAttackedBase=5\n");
    brains::s_AiParameters parameters;

    REQUIRE(AiParametersLoader().load(path.string(), parameters));
    std::filesystem::remove(path);

    brains::s_AiParameters expected;
    expected.shared.restTime = 20;
    expected.shared.respondersWhenHarvesterAttacked.base = 5;
    CHECK(parameters == expected);
    CHECK(parameters.shared.respondersWhenHarvesterAttacked.randomExtra == 4);
}

TEST_CASE("AiParametersLoader keeps the defaults without a section or file")
{
    const auto path = writeTempFile("[OTHER]\nRestTime=1\n");
    brains::s_AiParameters parameters;

    CHECK(AiParametersLoader().load(path.string(), parameters));
    std::filesystem::remove(path);
    CHECK(parameters == brains::s_AiParameters{});

    brains::s_AiParameters missing;
    CHECK_FALSE(AiParametersLoader().load("this_file_does_not_exist_ai.ini", missing));
    CHECK(missing == brains::s_AiParameters{});
}

TEST_CASE("The shipped AI.ini holds the default values")
{
    brains::s_AiParameters parameters;
    // start from values that differ from every default, so a key missing from the file shows up
    parameters.shared.restTime = -1;
    parameters.skirmish.initialRestRandom = -1;
    parameters.skirmish.mayBuildAdditionalUnitsOffset = {-1, -1};
    parameters.campaign.initialRestTime = -1;
    parameters.missions.buildOrderWaitBuildTimeFactor = -1.0f;
    parameters.sandworm.thinkInterval = -1;
    parameters.fremenSuperWeapon.keepStructureTargetAbove = -1;

    REQUIRE(AiParametersLoader().load(std::string(D2TM_SOURCE_DIR) + "/resources/bin/AI.ini", parameters));

    CHECK(parameters == brains::s_AiParameters{});
}
