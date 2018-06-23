#ifndef NND3D_GAME_STAGE_PROCS_LEGACYPROC_HPP
#define NND3D_GAME_STAGE_PROCS_LEGACYPROC_HPP
#pragma once

#include "../CharacterProc.hpp"

namespace nnd3d { namespace game {

// ---------------------------------------------------------------------------
// A factory function that makes processes.
// ---------------------------------------------------------------------------
gsl::owner<CharacterProc *> legacy_add_process(
    CharacterProcEnv & env, World & world, EntityManagerCO & em,
    const std::string & name,
    const SpriteLevelData & lvl_data);

}    }

#endif
