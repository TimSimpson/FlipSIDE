#include "World.hpp"
#include <algorithm>

namespace nnd3d {   namespace game {

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

World::World()
:   Gravity(0),
    clock(0),
    screen(),
    LemonTime(false),
    camera(),
    Sprite(),
    player_data(),
    numberPlayers(ActivePlayers::ap0()),
    game_state()
{
    for (int i = 0; i < lp3::narrow<int>(player_data.size()); ++ i) {
        player_data[i].index = i;
    }
    std::fill(Sprite.begin(), Sprite.end(), CharacterSprite{});

    // Taken from "StartUp"
    this->player_data[0].KeyUp = "W";
    this->player_data[0].KeyDown = "S";
    this->player_data[0].KeyLeft = "A";
    this->player_data[0].KeyRight = "D";
    this->player_data[0].KeyAttack = "F";
    this->player_data[0].KeyJump = "G";

    this->player_data[1].KeyUp = "up";
    this->player_data[1].KeyDown = "down";
    this->player_data[1].KeyLeft = "left";
    this->player_data[1].KeyRight = "right";
    this->player_data[1].KeyAttack = "O";
    this->player_data[1].KeyJump = "P";

    //KeyUp(2) = "I"
    //KeyDown(2) = "K"
    //KeyLeft(2) = "J"
    //KeyRight(2) = "L"
    //KeyAttack(2) = "Y"
    //KeyJump(2) = "U"


    //KeyUp = "W"
    //KeyDown = "S"
    //KeyLeft = "A"
    //KeyRight = "D"
    //KeyAttack = " "
    //KeyJump = "J"

    this->camera.set_size({640, 480});

    for (int j = 0; j < 100; ++j)
    {
        this->Sprite[j].visible = false;
    }
}

World::World(const World & other)
:   Gravity(other.Gravity),
    clock(other.clock),
    screen(other.screen),
    LemonTime(other.LemonTime),
    camera(other.camera),
    Sprite(other.Sprite),
    player_data(other.player_data),
    numberPlayers(other.numberPlayers),
    game_state(other.game_state),
    currentScreen(other.currentScreen)
{}

long anyKey(World & world, int zed) {
    // Returns true if the player at the given index is pressing any key.
    return ((world.player_data[zed].RightKEY || world.player_data[zed].LeftKEY
        || world.player_data[zed].upKey || world.player_data[zed].DownKEY
        || world.player_data[zed].AttackKey) ? 1 : 0);
}

}   }   // end namespace
