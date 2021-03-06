#ifndef NND3D_NOWLOADINGSCREEN_HPP
#define NND3D_NOWLOADINGSCREEN_HPP
#pragma once

#include <functional>
#include "Game.hpp"

namespace nnd3d { namespace game {


std::unique_ptr<GameProcess> create_now_loading_screen(
    GameContext context,
    std::function<std::unique_ptr<GameProcess> (GameContext context)> next_proc);



}   }  // end namespace

#endif
