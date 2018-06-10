#include "GameState.hpp"
#include <lp3/core.hpp>

namespace nnd3d { namespace game {

namespace {
    const ActivePlayers active_players[] = {
        { { false, false, false }, 0, { -1, 0, 0 } },
        { { true, false, false }, 1, { 0, 0, 0 } },
        { { true,  true, false }, 2, { -2, 0, 10 } },
        { { true,   true,  true }, 3, { -3, 0, 0 } },
        { { false,  true, false }, 4, { 10, 0, 0 } },
        { { false, false,  true }, 5, { 20, 0, 0 } },
        { { true, false,  true }, 6, { -2, 0, 20 } },
        { { false,  true,  true }, 7, { -2, 10, 20 } }
    };
}

const ActivePlayers & ActivePlayers::ap0() {
    return active_players[0];
}

const ActivePlayers & ActivePlayers::ap1() {
    return active_players[1];
}

const ActivePlayers & ActivePlayers::ap2() {
    return active_players[2];
}

const ActivePlayers & ActivePlayers::ap3() {
    return active_players[3];
}

const ActivePlayers & ActivePlayers::ap4() {
    return active_players[4];
}

const ActivePlayers & ActivePlayers::ap5() {
    return active_players[5];
}

const ActivePlayers & ActivePlayers::ap6() {
    return active_players[6];
}

const ActivePlayers & ActivePlayers::ap7() {
    return active_players[7];
}

const ActivePlayers & ActivePlayers::find_from_active_players(
        const std::array<bool, 3> & player) {
    for (const auto & element : active_players) {
        if (player[0] == element.player[0]
            && player[1] == element.player[1]
            && player[2] == element.player[2])
        {
            return element;
        }
    }
    return active_players[0];
}

GameState::GameState()
:   continue_count(2),
    numberPlayers(ActivePlayers::ap0())
{
    for (int i = 0; i < lp3::narrow<int>(player_data.size()); ++ i) {
        player_data[i].index = i;
    }
}

}   }
