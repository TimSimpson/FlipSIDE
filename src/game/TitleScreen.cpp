#include "TitleScreen.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "CharacterProc.hpp"
#include "LegacyGame.hpp"

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


class TitleScreenImpl : public GameProcess
{
private:
	Vb & vb;
	view::View & view;
	Sound & sound;
	World & world;

public:
	TitleScreenImpl(GameProcessSpace & space, 
		            view::View & view_arg, Sound & sound_arg, Vb & vb_arg,
                    World & world_arg)
    :   GameProcess(space),
		vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
        world(world_arg)		
    {
		world = World{};
		this->destroyEverything();
        world.screen = "title2";	
		this->titleScreen();
    }

    void handle_input(const input::Event & event) override {
        switch(event.key) {
            case input::Key::up:
                world.player_data[event.player].upKey = event.value != 0.0f;
                break;
            case input::Key::down:
                world.player_data[event.player].DownKEY = event.value != 0.0f;
                break;
            case input::Key::left:
                world.player_data[event.player].LeftKEY = event.value != 0.0f;
                break;
            case input::Key::right:
                world.player_data[event.player].RightKEY = event.value != 0.0f;
                break;
            case input::Key::attack:
                world.player_data[event.player].AttackKey = event.value != 0.0f;
                break;
            case input::Key::jump:
                world.player_data[event.player].JumpKey = event.value != 0.0f;
                break;
            default:
                break;
        }
    }

    void update() override {
        world.lasttime = world.clock + 3.33333333333333E-02;
        int j = 0;
        int k = 0;
        int penguin = 0;
       
        //Rem-FLICKER-
        for (j = 0; j < world.spritesInUse; ++j) {
            {
                auto & s = world.Sprite[j];

                if (s.trueVisible != 0) {
                    if (s.trueVisible == 1) { s.visible = true; }
                    if (s.trueVisible == 2) { s.visible = false; }
                    //s.trueVisible = 0
                }

                if (s.flickerTime > world.clock) {
                    if (s.trueVisible == 0) {
                        if (s.visible == false) { s.trueVisible = 2; }
                        if (s.visible == true) { s.trueVisible = 1; }
                    }

                    if (s.flickOn == true) { s.visible = false; }
                    if (s.flickOn == false) { s.visible = true; }
                    if (s.flickOn == true) {
                        s.flickOn = false;
                    }
                    else {
                        s.flickOn = true;
                    }
                }
            }
        }


        for (j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];


            if (s.name == "Title1") {
                if (s.miscTime < world.clock) {
                    if (s.miscTime + 3 > world.clock) {
                        s.visible = true;
                    } else {
                        s.visible = false;
                    }
                }
            }

            if (s.name == "TitleBg1") {
                //if (s.mode = "part2") then
                for (k = 0; k <= 2; ++ k) {
                    if (world.player_data[k].RightKEY == true || world.player_data[k].LeftKEY == true
                        || world.player_data[k].upKey == true || world.player_data[k].DownKEY == true
                        || world.player_data[k].AttackKey == true) {
                        world.screen = "Select Player";
                    }
                }

                //End If
                //if (s.mode <> "part2") then
                //if (sRightKEY = True Or LeftKEY = True Or UpKEY = True Or DownKEY = True Or AttackKey = True) then
                //For k = 1 To 4: Sprite(k).miscTime = clock - 100: Sprite(k).visible = False: Next k
                //End If
                //End If
                if (s.miscTime < world.clock && s.mode != "part2") {
                    s.visible = true;
                    s.mode = "part2"; //: .miscTime = clock + 5
                }
            }

			if (s.name == "Title2") {
                if (s.miscTime < world.clock && s.mode != "stop") {
                    s.trueVisible = 1;
                    s.flickerTime = world.clock + 5;
                    s.high = s.high - 5 * world.sFactor;
                    s.y = s.y + world.sFactor * 2.5;
                    if (s.high < 213) {
                        s.mode = "stop";
                        s.flickerTime = world.clock;
                    }
                }
            }

            if (s.name == "Title3") {
                if (s.miscTime < world.clock && s.mode == "stop") {
                    world.screen = "intro story";
                }
                if (s.miscTime < world.clock && s.mode != "stop") {
                    s.trueVisible = 1;
                    s.flickerTime = world.clock + 5;
                    s.wide = s.wide - world.sFactor * 5;
                    s.x = s.x + world.sFactor * 2.5;
                    if (s.wide < 640) {
                        s.mode = "stop";
                        s.flickerTime = world.clock;
                        s.miscTime = world.clock + 2; //: screen = "intro story"
                    }
                }
            }



            if (s.name == "intro story") {
                for (k = 0; k <= 2; ++ k) {
                    if (this->anyKey(k) == 1) {
                        world.screen = "Select Player";
                    }
                }
                if (s.mode == "") {

                    world.Sprite[1].invTime = 40;
                    sound.PlayWave("IntroStory.ogg"); // play it once then stop
                    s.mode = "waka do";  // so it won't load non stop
                }
                if (world.Sprite[1].mode == "words1") {
                    //Sprite(1).length = 6
                    if (s.mode == "waka do") {
                        //playWave "conSix.wav"
                        auto & s2 = world.Sprite[1];
                        s2.length = 3;
                        s2.texture = 1;
                        s2.visible = true;
                        //s2.srcx = 2: s2.srcy = 44;
                        //s2.srcx2 = 303: s2.srcy2 = 152;
                        s2.srcx = 2;
                        s2.srcy = 1;
                        s2.srcx2 = 166;
                        s2.srcy2 = 41;
                        s2.wide = 164 * 2;
                        s2.high = 40 * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 2") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 6;
                        s2.texture = 1;
                        s2.visible = true;
                        s2.srcx = 2;
                        s2.srcy = 44;
                        s2.srcx2 = 303;
                        s2.srcy2 = 152;
                        s2.wide = 301 * 2;
                        s2.high = (152 - 44) * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 3") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 2;
                        s2.texture = 1;
                        s2.visible = true;
                        s2.srcx = 2;
                        s2.srcy = 153;
                        s2.srcx2 = 123;
                        s2.srcy2 = 173;
                        s2.wide = 121 * 2;
                        s2.high = (20) * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 4") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 6;
                        s2.texture = 1;
                        s2.visible = true;
                        s2.srcx = 3;
                        s2.srcy = 174;
                        s2.srcx2 = 311;
                        s2.srcy2 = 263;
                        s2.wide = 308 * 2;
                        s2.high = (89) * 2;
                        s2.x = 1;
                        s2.y = 178;
                    }
                    if (s.mode == "word 5") {
                        s.length = 5;
                        view.LoadTexture(-1, "Open2.png", 320, 240);
                        world.CameraWidth = 320;
                        world.CameraHeight = 240;
                        {
                            auto & s2 = world.Sprite[1];
                            s2.texture = 2; //1
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 5;
                            s2.srcx2 = 307;
                            s2.srcy2 = 48;
                            s2.wide = 305 * 2;
                            s2.high = (43) * 2;
                            s2.x = 2 * 2;
                            s2.y = 173 * 2;
                        }
                    }
                    if (s.mode == "word 6") {
                        auto & s2 = world.Sprite[1];
                        s2.length = 3;
                        s2.texture = 2; //1
                        s2.visible = true;
                        s2.srcx = 2;
                        s2.srcy = 51;
                        s2.srcx2 = 311;
                        s2.srcy2 = 71;
                        s2.wide = 309 * 2;
                        s2.high = (20) * 2;
                        //.x = 1: .y = 178
                    }
                    if (s.mode == "word 7") {
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 4;
                            //s2.texture = 1;
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 75;
                            s2.srcx2 = 295;
                            s2.srcy2 = 117;
                            s2.wide = 293 * 2;
                            s2.high = (42) * 2;
                            //s2.x = 1: s2.y = 178;
                        } //end with
                    }
                    if (s.mode == "word 8") {
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 6;
                            //s2.texture = 1;
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 120;
                            s2.srcx2 = 294;
                            s2.srcy2 = 185;
                            s2.wide = 292 * 2;
                            s2.high = (65) * 2;
                            //s2.x = 1: s2.y = 178;
                        } //end with
                    }
                    if (s.mode == "word 9") {
                        view.LoadTexture(-1, "Open3.png", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            //.texture = 1
                            s2.length = 6;
                            s2.visible = true;
                            s2.srcx = 2;
                            s2.srcy = 189;
                            s2.srcx2 = 305;
                            s2.srcy2 = 254;
                            s2.wide = 303 * 2;
                            s2.high = (65) * 2;
                            //.x = 1: .y = 178
                        } //end with
                    }
                    if (s.mode == "word 10") {
                        view.LoadTexture(-1, "Open4.png", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 6;
                            s2.texture = 3;
                            s2.visible = true;
                            s2.length = 7;
                            s2.srcx = 1;
                            s2.srcy = 4;
                            s2.srcx2 = 312;
                            s2.srcy2 = 69;
                            s2.wide = 313 * 2;
                            s2.high = (65) * 2;
                            //.x = 1: .y = 178
                        } //end with
                    }
                    if (s.mode == "word 11") {
                        view.LoadTexture(-1, "Open5.png", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            s2.length = 6;
                            //s2.texture = 1;
                            s2.visible = true;
                            s2.srcx = 1;
                            s2.srcy = 72;
                            s2.srcx2 = 258;
                            s2.srcy2 = 115;
                            s2.wide = 259 * 2;
                            s2.high = (43) * 2;
                            s2.x = 58 * 2;
                            s2.y = 188 * 2;
                        }
                    }
                    if (s.mode == "word 12") {
                        //if (s.miscTime < clock) then
                        //killS (1)
                        view.LoadTexture(-1, "PlainBlack.png", 320, 240);
                        s.mode = "word 13";
                        {
                            auto & s2 = world.Sprite[1];
                            s2.invTime = 10;
                            s2.length = 6;
                            //s2.miscTime = 0;
                            s2.mode = "words1";
                            s2.texture = 4;
                            s2.visible = true;
                            s2.srcx = 1;
                            s2.srcy = 1;
                            s2.srcx2 = 320;
                            s2.srcy2 = 240;
                            s2.wide = 320 * 2;
                            s2.high = (240) * 2;
                            s2.x = 1;
                            s2.y = 1;
                            world.Sprite[0].mode = "KILLDEATH";
                        }
                        //End If
                    }

                }
                if (world.Sprite[1].mode == "words2") {
                    //playWave "conzero.wav"
                    world.Sprite[1].mode = "words1";
                    s.miscTime = 0;
                    if (s.mode == "word 11") {
                        s.mode = "word 12"; //: .miscTime = clock + 3: Sprite[1].length = 0
                    }
                    if (s.mode == "word 10") {
                        s.mode = "word 11"; //: Sprite[1].length = 0
                    }
                    if (s.mode == "word 9") { s.mode = "word 10"; }
                    if (s.mode == "word 8") { s.mode = "word 9"; }
                    if (s.mode == "word 7") { s.mode = "word 8"; }
                    if (s.mode == "word 6") { s.mode = "word 7"; }
                    if (s.mode == "word 5") { s.mode = "word 6"; }
                    if (s.mode == "word 4") { s.mode = "word 5"; }
                    if (s.mode == "word 3") { s.mode = "word 4"; }
                    if (s.mode == "word 2") { s.mode = "word 3"; }
                    if (s.mode == "waka do") { s.mode = "word 2"; }
                }
            }

            //WORDS 1
            if (s.name == "words1") {

                if (s.mode == "words1") {
                    if (s.miscTime < 255) {
                        s.miscTime = s.miscTime + (world.sFactor * s.invTime);
                        //0s.3 * sFactor
                        s.color = view.Rgb(s.miscTime, s.miscTime, s.miscTime);
                    }
                    if (s.miscTime > 245) {
                        //playWave "conten.wav"
                        s.color = view.QBColor(15);
                        s.mode = "words1b";
                        s.miscTime = world.clock + s.length;
                    }
                }

                if (world.Sprite[0].mode == "KILLDEATH" && s.mode == "words1b") {
                    s.name = "";
                }

                if (s.mode == "words1b" && s.miscTime < world.clock) {
                    s.mode = "words1c";
                    s.miscTime = 255;
                }

                if (s.mode == "words1c") {
                    if (s.miscTime > 0) {
                        s.miscTime = s.miscTime - (world.sFactor * 20); //0.3 * sFactor
                        if (s.miscTime > 0) {
                            s.color = view.Rgb(s.miscTime, s.miscTime, s.miscTime);
                        }
                    }
                    if (s.miscTime < 1) {
                        //playWave "conten.wav"
                        s.color = view.QBColor(0);
                        s.mode = "words2";
                    }
                }

            }



        }
        //TSNOW: end of the emulated with statement that creates variable "s",
        //       along with the for loop that used the "j" variable.
        //       Holy crap, after formatting that it's 1259 lines long.
        //       In Visual Basic it was 910 lines long.

        //---------------------------------------------------------------------
        //      END OF AN ERA
        //---------------------------------------------------------------------

        view.UpdateSprites();

        this->flipGame();       

		if (world.screen == "Select Player") {
			world.screen = "SelectPlayerz";
			this->exec(
				create_legacy_screen(
					get_process_space(), view, sound, vb, world));
		}
    }
   

private:
    long anyKey(int zed) {
        // Returns true if the player at the given index is pressing any key.
        return ((world.player_data[zed].RightKEY || world.player_data[zed].LeftKEY
                 || world.player_data[zed].upKey || world.player_data[zed].DownKEY
                 || world.player_data[zed].AttackKey) ? 1 : 0);
    }


    // 2017: Initializes the game. Port uses a lot of constructors so it misses
    // the sheer joy of initializing hundreds of global variables sitting in a
    // big static array like the old code did.
    void destroyEverything(int how=0) {
        int penguin;
        int goatorg;
        if (world.Sprite[1].name != "Selecter" 
			&& world.Sprite[0].name != "GameOverCloudBg") 
		{
			sound.silence_sfx();
        }

        sound.PlayIsoWave("nothing.wav");

        if (how != 2) {
            sound.PlayBgm("");
        }

        view.LoadTexture(-1, "PlainBlack.png", 25, 25);
        world.gotFocus = -1;
        world.CameraX = 0;
        world.CameraY = 0;
        goatorg = 0;
        penguin = world.spritesInUse;
        //If how = 2 Then goatorg = 30: penguin = 95

        for (int j = goatorg; j <= penguin; ++ j) {
            world.Sprite[j] = CharacterSprite{};
        }
    }

    void findOrder() {
        int texorg = 0;
        int davidorg1 = 0;

        //2017: This is madness. Looks like it set `drawTrue` to false for
        //      everything, then went through and found the sprites with the
        //      highest z order first and put them in the drawOrder array.
        //      At the end the drawOrder array has indices of sprites from
        //      the close (high z order) ones to the far away (low z order)
        //      ones.
        for (int j = 0; j <= world.spritesInUse; ++ j) {
         world.Sprite[j].drawTrue = false;
         world.drawOrder[j] = -150;
        }

        for (int j = 0; j <= world.spritesInUse; ++ j) {
         texorg = -150;
         davidorg1 = 0;
         for (int k = 0; k <= world.spritesInUse; ++ k) {
             if (world.Sprite[k].zOrder > texorg
                 && world.Sprite[k].drawTrue == false) {
                 texorg = world.Sprite[k].zOrder;
                 davidorg1 = k;
             }
         }
         world.drawOrder[j] = davidorg1;
         world.Sprite[davidorg1].drawTrue = true;
        }

        //2017: Sanity check
        int last_value = 9999;
        for (int i = 0; i <= world.spritesInUse; ++i) {
            int next_value = world.Sprite[world.drawOrder[i]].zOrder;
            LP3_ASSERT(last_value >= next_value);
            last_value = next_value;
        }
    }

    void flipGame() {
        // I think this handles switching to different rooms or levels.


        if (world.screen == "intro story") {
            world.screen = "intro story 2";
            destroyEverything();
            view.LoadTexture(1, "Open1.png", 313, 263);
            view.LoadTexture(2, "Open6.png", 320, 258);
            view.LoadTexture(3, "Open7.png", 320, 194);
            view.LoadTexture(4, "TitleScreen.png", 320, 240);
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

    }

    void titleScreen() {
        int j = 0;

        for (j = 0; j <= 2; ++j) {
            world.player_data[j].lives = 3;
            world.continues = 2;
        }

        this->destroyEverything();
        sound.PlayWave("OpeningWAV.wav");

        view.LoadTexture(0, "title2ALT.png", 285, 427);
        view.LoadTexture(1, "title1.png", 440, 427);
        {
            auto & s = world.Sprite[0];
            s.srcx = 5;
            s.srcy = 137;
            s.srcx2 = 324;
            s.srcy2 = 318;
            s.x = 1;
            s.y = 1;
            s.wide = 640;
            s.high = 480;
            s.visible = false;
            s.name = "TitleBg1";
            s.texture = 1;
            s.miscTime = world.clock + 20;
        }
        {
            auto & s = world.Sprite[1];
            s.srcx = 1;
            s.srcy = 3;
            s.srcx2 = 196;
            s.srcy2 = 107;
            s.x = 0;
            s.y = 180;
            s.wide = 193;
            s.high = 106;
            s.visible = false;
            s.name = "Title1";
            s.miscTime = world.clock + 2;
        }
        {
            auto & s = world.Sprite[2];
            s.srcx = 2;
            s.srcy = 111;
            s.srcx2 = 279;
            s.srcy2 = 230;
            s.x = 0;
            s.y = 174;
            s.wide = 277;
            s.high = 119;
            s.visible = false;
            s.name = "Title1";
            s.miscTime = world.clock + 6;
        }
        {
            auto & s = world.Sprite[3];
            s.srcx = 1;
            s.srcy = 233;
            s.srcx2 = 224;
            s.srcy2 = 363;
            s.x = 0;
            s.y = 168;
            s.wide = 232;
            s.high = 130;
            s.visible = false;
            s.name = "Title1";
            s.miscTime = world.clock + 10;
        }
        {
            auto & s = world.Sprite[4];
            s.srcx = 1;
            s.srcy = 366;
            s.srcx2 = 198;
            s.srcy2 = 424;
            s.x = 0;
            s.y = 228;
            s.wide = 197;
            s.high = 58;
            s.visible = false;
            s.name = "Title1";
            s.miscTime = world.clock + 14;
        }
        {
            auto & s = world.Sprite[5];
            s.srcx = 9;
            s.srcy = 6;
            s.srcx2 = 348;
            s.srcy2 = 81;
            s.x = 1;
            s.y = -240;
            s.wide = 640;
            s.high = 960;
            s.visible = false;
            s.name = "Title2";
            s.miscTime = world.clock + 20;
            s.texture = 1;
        }
        {
            auto & s = world.Sprite[6];
            s.srcx = 7;
            s.srcy = 91;
            s.srcx2 = 437;
            s.srcy2 = 128;
            s.x = -320;
            s.y = 140 + 213;
            s.wide = 1280;
            s.high = 110;
            s.visible = false;
            s.name = "Title3";
            s.miscTime = world.clock + 20;
            s.texture = 1;
        }

        this->findOrder();
    }

};  // end of GameImpl class


GameProcess * create_title_screen(GameProcessSpace & space,
                                  view::View & view, Sound & sound, Vb & vb, 
	                              World & world) {
	return new TitleScreenImpl(space, view, sound, vb, world);
}

}   }  // end namespace
