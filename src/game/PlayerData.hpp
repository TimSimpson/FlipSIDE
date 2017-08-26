#ifndef NND3D_PLAYERDATA_HPP
#define NND3D_PLAYERDATA_HPP
#pragma once

#include <string>
#include "CharacterSprite.hpp"

namespace nnd3d { namespace game {

struct PlayerData {
    //Keyboard key states
    bool upKey;
    bool DownKEY;
    bool LeftKEY;
    bool RightKEY;
    bool SelectKey;
    bool AttackKey;
    bool AttackKeyRelease;
    bool CancelKey;
    bool JumpKey;

    std::string KeyUp;
    std::string KeyDown;
    std::string KeyLeft;
    std::string KeyRight;
    std::string KeyAttack;
    std::string KeyJump;
    std::string playerName;
    int lives;
    bool ThreeWay;
    bool slicer;
    int GradeUp;
    std::string weapon;

    int sprite_index;
    CharacterSprite * sprite;
	int player_index;

    PlayerData();

	bool any_key_down() const;
};

}   }

#endif
