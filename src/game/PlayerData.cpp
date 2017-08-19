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
    weapon()
{
}

}	}   // end namespace