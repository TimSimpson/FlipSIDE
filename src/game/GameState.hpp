#ifndef NND3D_GAME_GAMESTATE_HPP
#define NND3D_GAME_GAMESTATE_HPP
#pragma once

#include <algorithm>
#include <array>
#include "PlayerData.hpp"

namespace nnd3d { namespace game {

constexpr int max_players = 3;

// This is based on some complicated magical number in the old code called
// "numberPlayers" which wasn't about the active player count but was a sort
// or enum that represented all combinations of active and inactive players.
class ActivePlayers {
public:
    std::array<bool, max_players> player;     // Who is or isn't activated.
    int numberPlayers;  // old magic number

    // Represents more magical constants
    struct FocusNonsense {
        int gotFocus;       // Yet another old magical number I don't understand.
        int j;
        int k;
    } focus;

    ActivePlayers(const ActivePlayers & other) = default;

    ActivePlayers(ActivePlayers && other) = default;

    ActivePlayers & operator=(const ActivePlayers & other) = default;

    bool any_player_active() const {
        return std::any_of(begin(player), end(player), [](bool b) { return b; });
    }

    static const ActivePlayers & ap0();
    static const ActivePlayers & ap1();
    static const ActivePlayers & ap2();
    static const ActivePlayers & ap3();
    static const ActivePlayers & ap4();
    static const ActivePlayers & ap5();
    static const ActivePlayers & ap6();
    static const ActivePlayers & ap7();

    static const ActivePlayers & find_from_active_players(
        const std::array<bool, 3> & player);
};


// ---------------------------------------------------------------------------
// class GameState
// ---------------------------------------------------------------------------
//     This is for all the persistant stuff that actually needs to stay
//     global for the course of a "game" (meaning from the time the game
//     starts ie after the title screen till the time it goes back to the
//     title screen).
//     This does not count things that either change or can be reinitialized
//     (or cached) between rooms, all of which will be managed elsewhere.
// ---------------------------------------------------------------------------
class GameState {
public:
    GameState();

    GameState(const GameState & other) = default;

    // TODO: Move all of the PlayerData stuff here

    inline int continues() const { return continue_count; }

    inline bool game_over() const { return continue_count < 0; }

    inline void use_continue() { --continue_count; }
private:
    int continue_count;

public:  // TODO: make these private
    std::array<PlayerData, max_players> player_data;

    ActivePlayers numberPlayers;
};

}   }

#endif
