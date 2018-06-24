#ifndef NND3D_GAME_PROCS_CHARACTERPROCFACTORY_HPP
#define NND3D_GAME_PROCS_CHARACTERPROCFACTORY_HPP
#pragma once

#include "../CharacterProc.hpp"
#include "../EntityManager.hpp"

namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_character_proc(
    CharacterProcEnv env, EntityManagerCO & e_manager,
    const std::string & name,
    const glm::dvec3 & position,
    const int texture_index);


}   }   }

#endif
