#include "Game.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "CharacterProc.hpp"

#include "TitleScreen.hpp"

#ifdef _MSC_VER
    // Avoid the zillions implicit conversion warnings
    #pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

namespace {
    const glm::vec4 normColor{1.0f, 1.0f, 1.0f, 1.0f};
}


GameProcessSpace::GameProcessSpace()
:   proc(nullptr)
{}

GameProcessSpace::~GameProcessSpace() {
	exec(nullptr);
}

void GameProcessSpace::exec(gsl::owner<GameProcess *> new_proc) {
    if (proc) {
        delete proc;
    }
    proc = new_proc;
}

GameProcess::GameProcess(GameProcessSpace & _space)
:	space(_space)
{}

Game::Game(view::View & _view, Sound & _sound, Vb & vb, Random & random, World & _world)
:   process(),
	world(_world)
{


    process.exec(create_title_screen(process, _view, _sound, vb, random, _world));
}

Game::~Game() {
}

void Game::handle_input(const input::Event & event) {
	switch (event.key) {
	case input::Key::quit:
		world.STOPGAME = true;
		break;
	case input::Key::lemon_time:
		world.LemonTime = true;
		break;
	default:
		process.get_proc()->handle_input(event);
	}
    
}

void Game::update() {
    process.get_proc()->update();
}

}   }  // end namespace
