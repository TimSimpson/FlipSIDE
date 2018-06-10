#include "PlayerData.hpp"

namespace nnd3d { namespace game {


PlayerData::PlayerData()
:   index(-1),
    active(false),
    upKey(false),
    DownKEY(false),
    LeftKEY(false),
    RightKEY(false),
    SelectKey(false),
    AttackKey(false),
    AttackKeyRelease(false),
    CancelKey(false),
    JumpKey(false),
    playerName(),
    lives(0),
    ThreeWay(false),
    slicer(false),
    GradeUp(0),
    weapon()
{
}

bool PlayerData::any_key() const {
    return RightKEY || LeftKEY || upKey || DownKEY || AttackKey;
}
}   }   // end namespace
