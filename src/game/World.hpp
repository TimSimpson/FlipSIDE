#ifndef NND3D_WORLD_HPP
#define NND3D_WORLD_HPP
#pragma once

#include <array>
#include "Camera.hpp"
#include "CharacterSprite.hpp"
#include "GameState.hpp"
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


// --------------------------------------------------------------------
// World
// --------------------------------------------------------------------
//      Contains the state of the "world", meaning the giant mess of
//      global variables the old code relied on.
// --------------------------------------------------------------------
struct World {
    World();

    World(const World & other);

    static constexpr int NUMSPRITES = 150;

    // TODO: this should also live in LegacyGame-
    //       and be initialized every time!
    Camera camera;

    // TODO: move this into LegacyGame -
    //       and let it initialize it every single time it's constructed! :O
    std::array<CharacterSprite, NUMSPRITES + 1> Sprite;

    // TODO: move this into `GameState`
    std::array<PlayerData, max_players> player_data;

    // TODO: move this into `GameState`
    ActivePlayers numberPlayers;

    GameState game_state;
};

long anyKey(World & world, int zed);

}   }

#endif
