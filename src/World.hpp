#ifndef NND3D_WORLD_HPP
#define NND3D_WORLD_HPP
#pragma once

#include <array>
#include "CharacterSprite.hpp"

namespace nnd3d {

// --------------------------------------------------------------------
// World
// --------------------------------------------------------------------
//      Contains the state of the "world", meaning the giant mess of
//      global variables the old code relied on.
// --------------------------------------------------------------------
struct World {
    World();

    static constexpr int FULLSCREENWIDTH = 640;
	static constexpr int FULLSCREENHEIGHT = 480;

    static constexpr int NUMSPRITES = 150;

    double Gravity;
    double clock;
    std::string screen;
    std::array<int, NUMSPRITES> drawOrder;

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
    int const tRate = 60; //125; //The target frame rate

    int CameraX;
    int CameraY;
    int CameraWidth;
    int CameraHeight;
    int cameraStopX;  //this is where the cameraHALTS!
    int cameraStopY; //this is where the cameraHALTS! for y
    int gotFocus;

    std::array<CharacterSprite, NUMSPRITES> Sprite;

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

    //Keyboard key states
    std::array<bool, 3> upKey;
    std::array<bool, 3> DownKEY;
    std::array<bool, 3> LeftKEY;
    std::array<bool, 3> RightKEY;
    std::array<bool, 3> SelectKey;
    std::array<bool, 3> AttackKey;
    std::array<bool, 3> AttackKeyRelease;
    std::array<bool, 3> CancelKey;
    std::array<bool, 3> JumpKey;


    int normColor;
    int maskColor;
    int spritesInUse;

    int ponkoMonkey;
    int dojoMonkey;

    std::array<std::string, 2> KeyUp;
    std::array<std::string, 2> KeyDown;
    std::array<std::string, 2> KeyLeft;
    std::array<std::string, 2> KeyRight;
    std::array<std::string, 2> KeyAttack;
    std::array<std::string, 2> KeyJump;

    std::string FilePath;

    //GAME PLAY MULTIPLE PLAYERS DATA
    int numberPlayers;
    std::array<std::string, 3> playerName;
    std::array<int, 3> lives;
    int continues;
    std::array<bool, 3> ThreeWay;
    std::array<bool, 3> slicer;
    std::array<int, 3> GradeUp;
    std::array<std::string, 3> weapon;

    std::string currentScreen;
};

}

#endif
