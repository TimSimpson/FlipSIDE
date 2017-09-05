#ifndef NND3D_GAME_HPP
#define NND3D_GAME_HPP
#pragma once

#include "../Input.hpp"
#include "../Random.hpp"
#include "../Sound.hpp"
#include "World.hpp"
#include "../Vb.hpp"
#include "../view.hpp"

namespace nnd3d { namespace game {

namespace core = lp3::core;
namespace gfx = lp3::gfx;

constexpr std::int64_t ms_per_update = 1000 / 60;  //16 ms for 60 fps
constexpr double speed_factor = ms_per_update / 1000.0;

class GameProcess;

class GameProcessSpace {
public:
    GameProcessSpace();

	~GameProcessSpace();

    void exec(gsl::owner<GameProcess *> proc);

    inline GameProcess * get_proc() {
        return proc;
    }
private:
    gsl::owner<GameProcess *> proc;
};

class GameProcess {
public:    
    virtual ~GameProcess() = default;

    // Drives the game
    virtual void handle_input(const input::Event &) = 0;

    // As a rule, this is called every 16ms.
	virtual gsl::owner<GameProcess *> update() = 0;
};


// --------------------------------------------------------------------
// Game
// --------------------------------------------------------------------
//      For now this is where the bulk of the code from NND3d2.bas
//      will live. The state from that mammoth module is represented
//      by the World struct.
// --------------------------------------------------------------------
class Game
{
public:
    Game(view::View & view, Sound & sound, Vb & vb, World & world);

	~Game();

    void handle_input(const input::Event & even);

    void update();

private:
    GameProcessSpace process;
	World & world;
};

}   }  // end namespace

#endif
