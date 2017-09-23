#ifndef NND3D_GAME_PROCS_PLAYERPROC_HPP
#define NND3D_GAME_PROCS_PLAYERPROC_HPP

#include "../CharacterProc.hpp"
#include "../EntityManager.hpp"

// void load_process(CharacterProcEnv env, CharacterSprite & s, const std::string &);
namespace nnd3d { namespace game { namespace proc {

CharacterProc * create_player_proc(
    CharacterProcEnv env, GameState & game_state,
    PlayerData & player_data, EntityManager & e_manager, const glm::vec2 & loc);

}   }   }

#endif
