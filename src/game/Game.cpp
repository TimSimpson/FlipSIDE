#include "Game.hpp"
#include <map>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

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
    lp3::core::GlobalVar<std::map<std::string, GameProcessEntry>> procs;
}

GameProcessSpace::GameProcessSpace()
:   proc(nullptr)
{}

GameProcessSpace::~GameProcessSpace() {
}

void GameProcessSpace::exec(gsl::owner<GameProcess *> new_proc) {
    LP3_ASSERT(new_proc);
    proc.reset(new_proc);
}


RegisterGameProcess::RegisterGameProcess(
    const char * const name,
    const char * const desc,
    gsl::owner<GameProcess *>(*create)(GameContext context))
{
    GameProcessEntry entry{ name, desc, create };
    procs.get()[std::string(name)] = entry;
}

std::vector<GameProcessEntry> get_all_game_processes() {
    std::vector<GameProcessEntry> entries;
    for (const auto & item : procs.get()) {
        entries.push_back(item.second);
    }
    return entries;
}

boost::optional<GameProcessEntry> get_game_process_by_name(const char * name) {
    auto itr = procs.get().find(name);
    if (itr == procs.get().end()) {
        return boost::none;
    } else {
        return itr->second;
    }
}

Game::Game(GameContext context, GameProcessEntry start_proc)
:   process(),
    _quit(false)
{
    process.exec(start_proc.create(context));
    if (_quit) {
        // If this isn't here, the game cuts out all of the static initializers
        // from these units! Here I was thinking I'd solved reflection in C++.
        // Time to go back to Macaroni. :p
        create_gameover_screen(context);
    }
    //process.exec(create_gameover_screen(_view, _sound, vb));
}

Game::~Game() {
}

void Game::handle_input(const input::Event & event) {
    switch (event.key) {
    case input::Key::quit:
        _quit = true;
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
