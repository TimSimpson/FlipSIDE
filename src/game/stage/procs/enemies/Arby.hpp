#ifndef NND3D_GAME_PROCS_ENEMIES_ARBY_HPP
#define NND3D_GAME_PROCS_ENEMIES_ARBY_HPP
#pragma once

#include "../../CharacterProc.hpp"
#include "../../EntityManager.hpp"

namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_arby_proc(
    CharacterProcEnv env, EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const int texture_index);


}   }   }

#endif
