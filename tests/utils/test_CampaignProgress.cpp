#include <catch2/catch_test_macros.hpp>

#include "game/CampaignProgress.hpp"
#include "include/definitions.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace {
class TemporaryDirectory {
public:
    TemporaryDirectory()
    {
        const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::temp_directory_path() / ("d2tm_campaign_progress_" + std::to_string(timestamp));
        std::filesystem::create_directories(m_path);
    }

    ~TemporaryDirectory()
    {
        std::filesystem::remove_all(m_path);
    }

    const std::filesystem::path& path() const
    {
        return m_path;
    }

private:
    std::filesystem::path m_path;
};
}

TEST_CASE("CampaignProgress tracks independent campaigns and persists unlocked missions", "[campaign]")
{
    TemporaryDirectory directory;
    const auto file = directory.path() / "progress.dat";
    CampaignProgress progress(file);

    std::string error;
    REQUIRE(progress.load(error) == CampaignProgressLoadResult::NotFound);
    REQUIRE(progress.nextMission(ATREIDES) == 0);
    REQUIRE(progress.nextMission(HARKONNEN) == 0);
    REQUIRE(progress.nextMission(ORDOS) == 0);

    REQUIRE(progress.markStarted(ATREIDES));
    REQUIRE(progress.recordMissionVictory(ATREIDES, 1));
    REQUIRE(progress.recordMissionVictory(ATREIDES, 2));
    REQUIRE(progress.markStarted(HARKONNEN));
    REQUIRE(progress.recordMissionVictory(HARKONNEN, 1));
    REQUIRE(progress.markStarted(ORDOS));

    REQUIRE(progress.nextMission(ATREIDES) == 3);
    REQUIRE(progress.highestUnlockedMission(ATREIDES) == 3);
    REQUIRE(progress.nextMission(HARKONNEN) == 2);
    REQUIRE(progress.highestUnlockedMission(HARKONNEN) == 2);
    REQUIRE(progress.nextMission(ORDOS) == 1);

    REQUIRE(progress.save(error));

    CampaignProgress loaded(file);
    REQUIRE(loaded.load(error) == CampaignProgressLoadResult::Loaded);
    REQUIRE(loaded.nextMission(ATREIDES) == 3);
    REQUIRE(loaded.nextMission(HARKONNEN) == 2);
    REQUIRE(loaded.nextMission(ORDOS) == 1);
}

TEST_CASE("CampaignProgress keeps unlocked missions when replaying an earlier mission", "[campaign]")
{
    TemporaryDirectory directory;
    CampaignProgress progress(directory.path() / "progress.dat");
    REQUIRE(progress.markStarted(ORDOS));
    REQUIRE(progress.recordMissionVictory(ORDOS, 3));

    REQUIRE_FALSE(progress.recordMissionVictory(ORDOS, 1));
    REQUIRE(progress.nextMission(ORDOS) == 4);
}

TEST_CASE("CampaignProgress marks the campaign complete and unlocks every mission", "[campaign]")
{
    TemporaryDirectory directory;
    CampaignProgress progress(directory.path() / "progress.dat");

    REQUIRE(progress.recordMissionVictory(HARKONNEN, CampaignProgress::LAST_MISSION));
    REQUIRE(progress.nextMission(HARKONNEN) == CampaignProgress::COMPLETED);
    REQUIRE(progress.highestUnlockedMission(HARKONNEN) == CampaignProgress::LAST_MISSION);
}

TEST_CASE("CampaignProgress rejects invalid progress files without loading partial data", "[campaign]")
{
    TemporaryDirectory directory;
    const auto file = directory.path() / "progress.dat";
    {
        std::ofstream output(file);
        output << "D2TM_CAMPAIGN_PROGRESS 1\n1 99 2 0\n";
    }

    CampaignProgress progress(file);
    std::string error;
    REQUIRE(progress.load(error) == CampaignProgressLoadResult::Invalid);
    REQUIRE_FALSE(error.empty());
    REQUIRE(progress.nextMission(ATREIDES) == 0);
    REQUIRE(progress.nextMission(HARKONNEN) == 0);
    REQUIRE(progress.nextMission(ORDOS) == 0);
}

TEST_CASE("CampaignProgress tracks the Sardaukar campaign independently and persists it", "[campaign]")
{
    TemporaryDirectory directory;
    const auto file = directory.path() / "progress.dat";
    CampaignProgress progress(file);

    REQUIRE(CampaignProgress::supportsHouse(SARDAUKAR));
    REQUIRE(progress.markStarted(SARDAUKAR));
    REQUIRE(progress.recordMissionVictory(SARDAUKAR, 2));
    REQUIRE(progress.nextMission(SARDAUKAR) == 3);
    REQUIRE(progress.nextMission(ATREIDES) == 0);

    std::string error;
    REQUIRE(progress.save(error));
    CampaignProgress loaded(file);
    REQUIRE(loaded.load(error) == CampaignProgressLoadResult::Loaded);
    REQUIRE(loaded.nextMission(SARDAUKAR) == 3);
}
