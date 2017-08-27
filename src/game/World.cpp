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
    CameraX(0),
    CameraY(0),
    CameraWidth(0),
    CameraHeight(0),
    cameraStopX(0),
    cameraStopY(0),
    gotFocus(0),
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

	// Taken from "StartUp"
	this->debugOn = false;
	//this->FilePath = App.Path + "\"";
	//this->levelPath = App.Path + "\"";

	this->player_data[0].KeyUp = "W";
	this->player_data[0].KeyDown = "S";
	this->player_data[0].KeyLeft = "A";
	this->player_data[0].KeyRight = "D";
	this->player_data[0].KeyAttack = "F";
	this->player_data[0].KeyJump = "G";

	this->player_data[1].KeyUp = "up";
	this->player_data[1].KeyDown = "down";
	this->player_data[1].KeyLeft = "left";
	this->player_data[1].KeyRight = "right";
	this->player_data[1].KeyAttack = "O";
	this->player_data[1].KeyJump = "P";

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

	this->CameraWidth = 640;  this->CameraHeight = 480;
	this->spritesInUse = World::NUMSPRITES;

	for (int j = 0; j < 100; ++j)
	{
		this->Sprite[j].visible = false;
	}

	//Time to get this show going, bodanky
	this->RealWidth = 640; //Form1.ScaleWidth;
	this->RealHeight = 480; //Form1.ScaleHeight;
	this->SimulatedWidth = World::FULLSCREENWIDTH;
	this->SimulatedHeight = World::FULLSCREENHEIGHT;
}

}	}   // end namespace
