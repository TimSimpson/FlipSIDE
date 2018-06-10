#include "PlayerData.hpp"

namespace nnd3d { namespace game {


PlayerData::PlayerData()
:   index(-1),
    active(false),
    playerName(),
    lives(0),
    ThreeWay(false),
    slicer(false),
    GradeUp(0),
    weapon()
{
}

}   }   // end namespace
