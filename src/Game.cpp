#include "Game.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

namespace input = lp3::input;

namespace nnd3d {

namespace {
    const glm::ivec4 normColor{255, 255, 255, 255};
}


class Game::GameImpl
{
public:
    GameImpl(lp3::input::Controls & _controls, View & _view, Sound & _sound,
             World & _world)
    :   controls(_controls),
        view(_view),
        sound(_sound),
        world(_world)
    {
        // Taken from "StartUp"
        world.debugOn = false;
        //world.FilePath = App.Path + "\"";
        //world.levelPath = App.Path + "\"";

        world.KeyUp[0] = "W";
        world.KeyDown[0] = "S";
        world.KeyLeft[0] = "A";
        world.KeyRight[0] = "D";
        world.KeyAttack[0] = "F";
        world.KeyJump[0] = "G";

        world.KeyUp[1] = "up";
        world.KeyDown[1] = "down";
        world.KeyLeft[1] = "left";
        world.KeyRight[1] = "right";
        world.KeyAttack[1] = "O";
        world.KeyJump[1] = "P";

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

        this->destroyEverything();
        world.screen = "title";
    }

    void PlayGame() {
        // TODO: FILL IN
    }

private:
    lp3::input::Controls & controls;
    View & view;
    Sound & sound;
    World & world;


    // 2017: Initializes the game. Port uses a lot of constructors so it misses
    // the sheer joy of initializing hundreds of global variables sitting in a
    // big static array like the old code did.
    void destroyEverything(int how=0) {
        int penguin;
        int goatorg;
        if (world.Sprite[1].name == "Selecter") {
            goto dogyup;
        }
        if (world.Sprite[0].name == "GameOverCloudBg") {
            goto dogyup;
        }

        for (int j = 1; j < 15; ++ j) {
            sound.PlayWave("nothing.wav");
        }

     dogyup:

        sound.PlayIsoWave("nothing.wav");

        if (how != 2) {
            sound.PlayBgm("");
        }

        view.LoadTexture(-1, "plainblack.bmp", 25, 25);
        //bgtexture = Nothing
        //for (int j = 0; j < 9; ++ j) {
            // Set AnimationTexture(j) = Nothing
        //}
        world.gotFocus = -1;
        world.CameraX = 0;
        world.CameraY = 0;
        goatorg = 0;
        penguin = world.spritesInUse;
        //If how = 2 Then goatorg = 30: penguin = 95

        for (int j = goatorg; j < penguin; ++ j) {
            world.Sprite[j] = CharacterSprite{};
        }
    }

    void findOrder() {
        int texorg = 0;
        int davidorg1 = 0;
        int k = 0;

        for (int j = 0; j < world.spritesInUse; ++ j) {
         world.Sprite[j].drawTrue = false;
         world.drawOrder[j] = -150;
        }

        for (int j = 0; j < world.spritesInUse; ++ j) {
         texorg = -150;
         davidorg1 = 0;
         for (int k = 0; k < world.spritesInUse; ++ k) {
             if (world.Sprite[k].zOrder > texorg
                 && world.Sprite[k].drawTrue == false) {
                 texorg = world.Sprite[k].zOrder;
                 davidorg1 = k;
             }
         }
         world.drawOrder[j] = davidorg1;
         world.Sprite[davidorg1].drawTrue = true;
        }
    }

    void flipGame() {
        // I think this handles switching to different rooms or levels.
        int penguin;
		(void)penguin;  //2017- is this unused?

        if (world.exitS == "true" && boost::starts_with(world.screen, "level")) {
            double sapple = boost::lexical_cast<double>(world.screen.substr(5));
            sapple = sapple + 0.1; // WTF, right? It's in the original code though...
            world.screen = str(boost::format("level%s") % sapple);
        } // End If

        if (world.screen == "intro story") {
            world.screen = "intro story 2";
            destroyEverything();
            view.LoadTexture(1, "open1.bmp", 313, 263);
            view.LoadTexture(2, "open6.bmp", 320, 258);
            view.LoadTexture(3, "open7.bmp", 320, 194);
            view.LoadTexture(4, "titlescreen.bmp", 320, 240);
            sound.PlayBgm("");
            world.Sprite[0].name = "intro story";
            {
                auto & s = world.Sprite[1];
                s.name = "words1";
                s.x = 1;
                s.y = 175;
                s.miscTime = 0;
                s.mode = "words1";
                s.visible = false;
                s.color = view.QBColor(0);
            }

            world.Sprite[0].color = view.QBColor(0);
            world.Sprite[0].visible = false;
        }  //End of intro story with


        if (world.screen == "SelectPlayerz") {
            this->selectPlayerS();
        }

        //playWave "conzero.wav"
        if (world.screen == "title") {
            //playWave "conTen.wav"
            world.screen = "title2";
            this->titleScreen();
            //screen = "intro story"
        }

        //************************************************************
        // LOADS A NEW LEVEL!------------------------------------
        //************************************************************
        if (boost::starts_with(world.screen, "level")
            && world.currentScreen != world.screen) {
            //as long as this is set to this crazy value, it won't load it again.
            world.currentScreen = world.screen;

            std::string crapple = world.screen.substr(5);
            double boogeycrap = boost::lexical_cast<double>(crapple);

            this->goToLevel(boogeycrap);
        }

        if (world.screen == "Select Player") {
            world.screen = "SelectPlayerz";
            this->selectPlayer();
        }

        if (world.screen == "deadscreen") {
            world.screen = "title";
        }

        if (world.Sprite[0].name == "dead"
            && world.Sprite[10].name == "dead"
            && world.Sprite[20].name == "dead") {
            this->gameOver();
        }
    }

	void gameOver() {}  // TODO
	void goToLevel(double) {} // TODO
	void selectPlayer() {} // TODO
	void selectPlayerS() {} // TODO
	void titleScreen() {} // TODO

};	// end of GameImpl class

Game::Game(input::Controls & _controls, View & _view, Sound & _sound,
           World & _world)
:   impl(new GameImpl(_controls, _view, _sound, _world)) {
}

Game::~Game() {
    delete impl;
}

void Game::PlayGame() {
    impl->PlayGame();
}

}   // end namespace
