#ifndef NND3D_GAME_PROCS_CINEMAPROC_HPP
#define NND3D_GAME_PROCS_CINEMAPROC_HPP
#pragma once

#include "../CharacterProc.hpp"
#include "../EntityManager.hpp"

namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_cinema_proc(
    CharacterProcEnv env, EntityManagerCO e_manager,
    const float stage);



}   }   }

#endif
