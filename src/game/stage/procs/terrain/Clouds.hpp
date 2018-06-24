#ifndef NND3D_GAME_PROCS_TERRAIN_CLOUDS_HPP
#define NND3D_GAME_PROCS_TERRAIN_CLOUDS_HPP
#pragma once

#include "../../CharacterProc.hpp"
#include "../../EntityManager.hpp"

namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_clouds_proc(
    CharacterProcEnv env,
    EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const glm::dvec3 & size,
    const int texture_index);


}   }   }

#endif
