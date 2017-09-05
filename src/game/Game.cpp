#include "Game.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "CharacterProc.hpp"

#include "TitleScreen.hpp"
#include "GameOverScreen.hpp"

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
	if (proc) {
		delete proc;
	}
}

void GameProcessSpace::exec(gsl::owner<GameProcess *> new_proc) {
	LP3_ASSERT(new_proc);
    if (proc) {
        delete proc;
    }
    proc = new_proc;
}

Game::Game(view::View & _view, Sound & _sound, Vb & vb)
:   process(),
	_quit(false)
{
    process.exec(create_title_screen(_view, _sound, vb));
	//process.exec(create_gameover_screen(_view, _sound, vb));
}

Game::~Game() {
}

void Game::handle_input(const input::Event & event) {
	switch (event.key) {
	case input::Key::quit:
		_quit = true;
		break;
	case input::Key::lemon_time:
		// TODO: fix Lemon time. For now, I want to stop World from spreading.
		// world.LemonTime = true;
		break;
	default:
		process.get_proc()->handle_input(event);
	}
    
}

bool Game::quit() const {
	return _quit;
}

void Game::update() {
    auto result = process.get_proc()->update();
	if (result) {
		process.exec(result);
	}
}


}   }  // end namespace
