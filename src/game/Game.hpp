#ifndef NND3D_GAME_HPP
#define NND3D_GAME_HPP
#pragma once

#include "../Input.hpp"
#include "../Random.hpp"
#include "../Sound.hpp"
#include "World.hpp"
#include "../Vb.hpp"
#include "../View.hpp"

namespace nnd3d { namespace game {

namespace core = lp3::core;
namespace gfx = lp3::gfx;


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
    GameProcess(GameProcessSpace & space);

    virtual ~GameProcess() = default;

    // Drives the game
    virtual void handle_input(const input::Event &) = 0;

    // As a rule, this is called every 16ms.
    virtual void update() = 0;

protected:
    // Replace the currently executing game process with a different one.
    void exec(gsl::owner<GameProcess *> proc) {
		space.exec(proc);
    }

private:
    GameProcessSpace & space;
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
    Game(view::View & view, Sound & sound, Vb & vb, Random & random, World & world);

	~Game();

    void handle_input(const input::Event & even);

    void update();

private:
    GameProcessSpace process;
};

}   }  // end namespace

#endif
