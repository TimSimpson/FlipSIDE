#include "PlayerData.hpp"

namespace nnd3d { namespace game {


PlayerData::PlayerData()
:   upKey(false),
    DownKEY(false),
    LeftKEY(false),
    RightKEY(false),
    SelectKey(false),
    AttackKey(false),
    AttackKeyRelease(false),
    CancelKey(false),
    JumpKey(false),
    KeyUp(),
    KeyDown(),
    KeyLeft(),
    KeyRight(),
    KeyAttack(),
    KeyJump(),
    playerName(),
    lives(0),
    ThreeWay(false),
    slicer(false),
    GradeUp(0),
    weapon(),
    sprite_index(-1),
    sprite(nullptr),
	player_index(-1)
{
}

bool PlayerData::any_key_down() const {
	// Returns true if the player at the given index is pressing any key.
	return (RightKEY || LeftKEY || upKey || DownKEY || AttackKey);
}

}	}   // end namespace
