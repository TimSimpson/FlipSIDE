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

	void gameOver() {
        sound.PlayBgm("");
        world.screen = "gameOver";
        this->destroyEverything();
        view.LoadTexture(0, "GameOver.bmp", 320, 287);
        {
            auto & s = world.Sprite[0];
            s.srcx = 1; s.srcy = 1; s.srcx2 = 320; s.srcy2 = 240;
            s.x = 0; s.y = 0; s.wide = 640; s.high = 480; s.visible = true;
            s.trueVisible = 1;
            s.name = "GameOverCloudBg";
            s.texture = 0;
            s.color = normColor;
        }
        {
            auto & s = world.Sprite[1];
            s.srcx = 1; s.srcy = 243; s.srcx2 = 320; s.srcy2 = 287;
            s.x = 0; s.y = 180; s.wide = 640; s.high = 94; s.visible = true;
            s.trueVisible = 1;
            s.name = "GameOverCloudTitle"; s.texture = 0;
            s.miscTime = world.clock + 4;
        }
        sound.PlayWave("gameover.wav");
        this->findOrder();
    }

	void goToLevel(const double which) {
        world.Gravity = 0;

        if (which == 1.1 || which == 1) {
            this->destroyEverything();
            this->MakeLevel("Level1Opening.wav", "level1.cap", "lv1bg.bmp",
                            10, 10, "level1cinema.bmp", "level1cinema.ani",
                            true, true);
            world.cinemaMax = 2;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1a1.wav", 4.49);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1a2.wav", 1.87);
            world.Sprite[0].x = 50;
            world.Sprite[0].y = 220;
            world.Sprite[10].x = 50;
            world.Sprite[10].y = 220;
            world.Sprite[20].x = 50;
            world.Sprite[20].y = 220;
            world.cameraStopX = 1010;
            world.cameraStopY = 905 + 480;

            sound.LoadSound(16, "goomba.wav", "dying explosion");
            sound.LoadSound(17, "goombaouch.wav", "Goomba Ouch");
            sound.LoadSound(18, "KerboDie.wav", "Kerbose Die");
            sound.LoadSound(19, "KerbHurt.wav", "Kerbose Ouch");
            sound.LoadSound(20, "putulohurt.wav", "putulohurt");
            sound.LoadSound(21, "putulodie.wav", "putulodie");
        }

        if (which == 1.2) {
            this->MakeLevel("level1.wav", "level1b.cap", "lv1bg2.bmp",
                            100, 100, "Level1BCinema.bmp", "level1bcinema.ani",
                            true, true);
            world.cinemaMax = 3;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3);
            this->setCinema(2, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05);
            world.Sprite[0].x = 1122;
            world.Sprite[0].y = 1650;
            world.Sprite[10].x = 1122;
            world.Sprite[10].y = 1650;
            world.Sprite[20].x = 1122;
            world.Sprite[20].y = 1650;
            world.cameraStopX = 1194;
            world.cameraStopY = 1900;
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "BS Death.wav", "Paul Shrink");
            sound.LoadSound(20, "WhaWhee.wav", "Stick Awaken");
        }

        if (which == 1.3) {
            this->MakeLevel("level1.wav", "level1c.cap", "lv1bg3.bmp",
                            10, 10, "Level1BCinema.bmp", "level1bcinema.ani",
                            false, false);
            world.cinemaMax = 3;
            world.cinemaCounter = 4;
            this->setCinema(0, 7, 7, 7, 7, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31);
            this->setCinema(1, 7, 7, 7, 7, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3);
            this->setCinema(2, 7, 7, 7, 7, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05);
            world.Sprite[0].x = 242;
            world.Sprite[0].y = 2000;
            world.Sprite[10].x = 42;
            world.Sprite[10].y = 300;
            world.Sprite[20].x = 42;
            world.Sprite[20].y = 300;
            world.cameraStopX = 1244;
            world.cameraStopY = 2273;
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "WhaWhee.wav", "Stick Awaken");
            sound.LoadSound(20, "BS Death.wav", "Paul Shrink");
        }

        if (which == 1.4) {
            this->MakeLevel("level1.wav", "level1d.cap", "level1birdstreet.bmp",
                            98, 480, "Level1DCinema.bmp", "level1bcinema.ani",
                            false, false);
            world.cinemaMax = 3;
            world.cinemaCounter = 4;
            this->setCinema(0, 7, 7, 7, 7, 8, 8, 15, 8, "TalkLv1c1.wav", 3.31);
            this->setCinema(1, 7, 7, 7, 7, 8, 15, 8, 8, "TalkLv1c2.wav", 2.3);
            this->setCinema(2, 7, 7, 7, 7, 15, 8, 8, 8, "TalkLv1c3.wav", 3.05);
            world.Sprite[0].x = 42;
            world.Sprite[0].y = 300;
            world.Sprite[10].x = 42;
            world.Sprite[10].y = 300;
            world.Sprite[20].x = 42;
            world.Sprite[20].y = 300;
            world.cameraStopX = 3000;
            world.cameraStopY = 480;
            sound.LoadSound(16, "BShurt.wav", "Stick Ouch");
            sound.LoadSound(17, "BS Death2.wav", "stick die");
            sound.LoadSound(18, "PaulHurt.wav", "Paul Ouch");
            sound.LoadSound(19, "WhaWhee.wav", "Stick Awaken");
            sound.LoadSound(20, "BS Death.wav", "Paul Shrink");
        }

        if (world.Gravity == 0) {
            // because Gravity is set to 20 in make level, we know that there
            // has been no other levels selected
            // in other words, somebody screwed up
            world.screen = "intro story"; // this will remind them that they failed
            // Note from present day: sure, just tell yourself that kid.
        }
    }

	template<typename... Args>
	void MakeLevel(Args&&... args) {} // TODO

    void NowLoading() {
        view.LoadTexture(-1, "nowloading.bmp", 320, 240);
        world.CameraWidth = 320;
        world.CameraHeight = 240;
        view.UpdateSprites();
        //Call DrawStuff
        world.CameraWidth = 640;
        world.CameraHeight = 480;
        view.ForceShowBackground();
    }

	void selectPlayer() {} // TODO
	void selectPlayerS() {} // TODO

	template<typename... Args>
	void setCinema(Args&&... args) {} // TODO

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
