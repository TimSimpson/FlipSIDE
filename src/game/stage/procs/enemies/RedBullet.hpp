#ifndef NND3D_GAME_PROCS_ENEMIES_REDBULLET_HPP
#define NND3D_GAME_PROCS_ENEMIES_REDBULLET_HPP
#pragma once

#include "../../CharacterProc.hpp"
#include "../../EntityManager.hpp"

namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_redbullet_proc(
    CharacterProcEnv env, EntityManagerCO e_manager,
    const glm::dvec3 & position,
    const glm::dvec3 & seek_location,
    const int texture_index);


}   }   }

#endif
