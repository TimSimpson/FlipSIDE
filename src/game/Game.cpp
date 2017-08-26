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
    // Taken from "StartUp"
    world.debugOn = false;
    //world.FilePath = App.Path + "\"";
    //world.levelPath = App.Path + "\"";

    world.player_data[0].KeyUp = "W";
    world.player_data[0].KeyDown = "S";
    world.player_data[0].KeyLeft = "A";
    world.player_data[0].KeyRight = "D";
    world.player_data[0].KeyAttack = "F";
    world.player_data[0].KeyJump = "G";

    world.player_data[1].KeyUp = "up";
    world.player_data[1].KeyDown = "down";
    world.player_data[1].KeyLeft = "left";
    world.player_data[1].KeyRight = "right";
    world.player_data[1].KeyAttack = "O";
    world.player_data[1].KeyJump = "P";

    //KeyUp(2) = "I"
    //KeyDown(2) = "K"
    //KeyLeft(2) = "J"
    //KeyRight(2) = "L"
    //KeyAttack(2) = "Y"
    //KeyJump(2) = "U"


    //KeyUp = "W"
    //KeyDown = "S"
    //KeyLeft = "A"
    //KeyRight = "D"
    //KeyAttack = " "
    //KeyJump = "J"

    world.CameraWidth = 640;  world.CameraHeight = 480;
    world.spritesInUse = World::NUMSPRITES;

    for (int j = 0; j < 100; ++ j)
    {
        world.Sprite[j].visible = false;
    }

    //Time to get this show going, bodanky
    world.RealWidth = 640; //Form1.ScaleWidth;
    world.RealHeight = 480; //Form1.ScaleHeight;
    world.SimulatedWidth = World::FULLSCREENWIDTH;
    world.SimulatedHeight = World::FULLSCREENHEIGHT;

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
