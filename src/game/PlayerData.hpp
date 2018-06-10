#ifndef NND3D_PLAYERDATA_HPP
#define NND3D_PLAYERDATA_HPP
#pragma once

#include <string>

namespace nnd3d { namespace game {

struct PlayerData {
    int index;

    bool active;

    std::string playerName;
    int lives;
    bool ThreeWay;
    bool slicer;
    int GradeUp;
    std::string weapon;

    PlayerData();
};

}   }

#endif
