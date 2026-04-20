/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#pragma once

#include <array>
#include <memory>
class cGameSettings;
struct s_DataCampaign;
class cMouse;
class cSideBarFactory;
#include "cPlayer.h"

/**
 * Manager class that encapsulates the global g_Player array
 * Provides safe access to players with bounds checking and convenient methods
 */
class cPlayers {
public:
    // Maximum number of players
    static constexpr int MAX_PLAYERS_CAPACITY = 7;

    /**
     * Default constructor
     */
    cPlayers();

    /**
     * Non-copyable
     */
    cPlayers(const cPlayers&) = delete;
    cPlayers& operator=(const cPlayers&) = delete;

    /**
     * Access player by index (compatible with array syntax)
     * @param index The player index
     * @return Reference to cPlayer object
     */
    cPlayer& operator[](int index);
    const cPlayer& operator[](int index) const;

    /**
     * Safe access with bounds checking
     * @param index The player index
     * @return Pointer to cPlayer if valid, nullptr otherwise
     */
    cPlayer* getPlayer(int index);
    const cPlayer* getPlayer(int index) const;

    /**
     * Get human player (player 0)
     * @return Reference to human player
     */
    cPlayer& getHumanPlayer();
    const cPlayer& getHumanPlayer() const;

    /**
     * Get the capacity (maximum number of players)
     * @return The capacity
     */
    static constexpr int capacity() { return MAX_PLAYERS_CAPACITY; }

    /**
     * Get the number of players (always equals capacity for now)
     * @return The size
     */
    static constexpr int size() { return MAX_PLAYERS_CAPACITY; }

    /**
     * Begin iterator for range-based for loops
     */
    auto begin() { return m_players.begin(); }
    auto begin() const { return m_players.begin(); }

    /**
     * End iterator for range-based for loops
     */
    auto end() { return m_players.end(); }
    auto end() const { return m_players.end(); }

    /**
     * Initialize all players with their brains and settings
     * @param rememberHouse Whether to preserve the player's house assignment
     * @param gameSettings Game settings (for AI and difficulty configuration)
     * @param dataCampaign Campaign data (for campaign-specific AI brain initialization)
     */
    void initPlayers(bool rememberHouse, cGameSettings* gameSettings, s_DataCampaign* dataCampaign);

    void setupPlayers(std::shared_ptr<cHousesInfo> housesInfo);

    // Initialize per-player runtime helpers used during gameplay.
    void setupRuntimePlayerComponents(cSideBarFactory* sideBarFactory, cMouse* mouse, int techLevel);

    void destroyAllegroBitmaps();

    void onNotifyGameEvent(const s_GameEvent& event);
    void evaluateStillAliveForAI();
private:
    std::array<cPlayer, MAX_PLAYERS_CAPACITY> m_players;
};
