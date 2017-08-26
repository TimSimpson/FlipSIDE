#include "World.hpp"
#include <algorithm>

namespace nnd3d {	namespace game {

const int World::FULLSCREENWIDTH = 640;
const int World::FULLSCREENHEIGHT = 480;

World::World()
:   Gravity(0),
    clock(0),
    screen(),
    drawOrder(),
    LemonTime(false),
    cranBerry(0),
    lasttime(0),
    frRate(0),
    frRate2(0),
    gpRate(0),
    gpRate2(0),
    cranBerry2(0),
    debugOn(false),
    sFactor(0),
    camera(),
    cinema({}),
    cinemaCounter(0),
    cinemaMax(0),
    exitS(),
    Sprite(),
    bgWidth(0),
    bgHeight(0),
    STOPGAME(false),
    RealWidth(0),
    RealHeight(0),
    SimulatedWidth(0),
    SimulatedHeight(0),
	player_data(),
    normColor(0),
    maskColor(0),
    spritesInUse(0),
    ponkoMonkey(0),
    dojoMonkey(0),
    FilePath(),
    numberPlayers(0),
    continues(0),
    currentScreen()
{
    std::fill(drawOrder.begin(), drawOrder.end(), 0);
    std::fill(Sprite.begin(), Sprite.end(), CharacterSprite{});
}

}	}   // end namespace
