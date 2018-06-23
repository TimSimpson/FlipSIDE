#include "CinemaProc.hpp"
#include <lp3/sims.hpp>
#include "enemies/Kerbose.hpp"

namespace nnd3d { namespace game { namespace proc {


CharacterProc * create_enemy_proc(
    CharacterProcEnv env,
    EntityManager & e_manager,
    const std::string & name,
    const glm::dvec3 & position,
    const int texture_index)
{
    if (name == "Kerbose" || name == "Kirbose") {
        return create_kerbose_proc(env, e_manager, position, texture_index);
    }
    return nullptr;
}

}   }   }
