#ifndef NND3D_WORLD_HPP
#define NND3D_WORLD_HPP
#pragma once

#include <array>
#include "Camera.hpp"
#include "CharacterSprite.hpp"
#include "CinemaType.hpp"
#include "PlayerData.hpp"

namespace nnd3d { namespace game {

// --------------------------------------------------------------------
// World
// --------------------------------------------------------------------
//      Contains the state of the "world", meaning the giant mess of
//      global variables the old code relied on.
// --------------------------------------------------------------------
struct World {
    World();

    static const int FULLSCREENWIDTH;
	static const int FULLSCREENHEIGHT;

    static constexpr int NUMSPRITES = 150;

    double Gravity;
    double clock;
    std::string screen;
    std::array<int, NUMSPRITES + 1> drawOrder;

    bool LemonTime; // a fun stupid cheat code that when set to true ups speed by 2.
    double cranBerry; //Used to gage frame rate
    double lasttime;
    int frRate;
    int frRate2;
    int gpRate;
    int gpRate2;
    double cranBerry2;
    bool debugOn;
    double sFactor; //makes objects move at speeds irrelevent to frame rate

    // Note from 2017: decreased from 125

    CameraGuts camera;
    int gotFocus;

	std::array<CinemaType, 40> cinema; // Dialogue!
	int cinemaCounter;
	int cinemaMax; //how many lines of dialogue per cinema
	std::string exitS;

    std::array<CharacterSprite, NUMSPRITES + 1> Sprite;

    int bgWidth;
    int bgHeight;

    //Used to get the heck out of a never ending do loop
    bool STOPGAME;

    //Used to figure out are width and heighth
    int RealWidth;
    int RealHeight;
    //Simulated resolution (of course, the real resolution)
    int SimulatedWidth;
    int SimulatedHeight;

    std::array<PlayerData, 3> player_data;


    int normColor;
    int maskColor;
    int spritesInUse;

    int ponkoMonkey;
    int dojoMonkey;


    std::string FilePath;

    //GAME PLAY MULTIPLE PLAYERS DATA
    int numberPlayers;
    int continues;

    std::string currentScreen;
};

long anyKey(World & world, int zed);

}   }

#endif
