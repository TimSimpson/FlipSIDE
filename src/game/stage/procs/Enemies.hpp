#ifndef NND3D_GAME_PROCS_ENEMIESPROC_HPP
#define NND3D_GAME_PROCS_ENEMIESPROC_HPP

#include "../CharacterProc.hpp"
#include "../EntityManager.hpp"

namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_enemy_proc(
    CharacterProcEnv env, EntityManager & e_manager,
    const std::string & name,
    const glm::dvec3 & position,
    const int texture_index);


}   }   }

#endif
