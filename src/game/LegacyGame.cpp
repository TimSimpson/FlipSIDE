#include "LegacyGame.hpp"
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


class LegacyGame : public GameProcess
{
private:
	Vb & vb;
	view::View & view;
	Sound & sound;
	World & world;
	Random random;

public:
	LegacyGame(GameProcessSpace & space, view::View & view_arg,
		       Sound & sound_arg, Vb & vb_arg, World & world_arg)
    :   GameProcess(space),
		vb(vb_arg),
        view(view_arg),
        sound(sound_arg),
        world(world_arg),
        random()
    {
		LP3_ASSERT(boost::starts_with(world.screen, "Level"));
		LP3_ASSERT(world.currentScreen != world.screen);

        //as long as this is set to this crazy value, it won't load it again.
        world.currentScreen = world.screen;

        std::string crapple = world.screen.substr(5);
        double boogeycrap = boost::lexical_cast<double>(crapple);

        this->goToLevel(boogeycrap);
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
            case input::Key::skip_scene:
                if (event.value) {
                    world.exitS = "true"; sound.PlayWave("FDis.wav");
                }
                break;
            case input::Key::power_up:
                world.player_data[0].slicer = true;
                world.player_data[0].GradeUp = 2;
                world.Sprite[0].wide = 25;
                world.Sprite[0].high = 25;
                sound.PlayWave("SoupedUp.wav");
                break;
            case input::Key::lemon_time:
                world.LemonTime = true;
                break;
            default:
                LP3_ASSERT(false);
        }
    }

    void update() override {
        world.lasttime = world.clock + 3.33333333333333E-02;
        int j = 0;
        int k = 0;
        int penguin = 0;
        int trueorg; //penguin and true org are buddies and also junk variables

                     //----------------------------------------------------------------------
                     //                   CAMERA TIME
                     //----------------------------------------------------------------------
        this->findPlayers();
        //If Sprite(0).name <> playerName(0) And Sprite(10).name <> playerName(1) And Sprite(20).name <> playerName(2) Then gotFocus = -6
        if (world.numberPlayers == 0) { world.gotFocus = -1; }
        if (world.numberPlayers == 1) { world.gotFocus = 0; }
        if (world.numberPlayers == 2) { world.gotFocus = -2; j = 0; k = 10; }
        if (world.numberPlayers == 3) { world.gotFocus = -3; }
        if (world.numberPlayers == 4) { world.gotFocus = 10; }
        if (world.numberPlayers == 5) { world.gotFocus = 20; }
        if (world.numberPlayers == 6) { world.gotFocus = -2; j = 0; k = 20; }
        if (world.numberPlayers == 7) { world.gotFocus = -2; j = 10; k = 20; }
        //1 Only player 1
        //2 Player 1 and 2
        //3 All three Players
        //4 Just player 2
        //5 Just player 3
        //6 Players 1 and 3
        //7 Players 2 and 3

        //Three Player Scrolling is kind of tricky...
        if (world.gotFocus == -3) {
            if (world.Sprite[0].x < world.Sprite[10].x
                && world.Sprite[0].x < world.Sprite[20].x) {
                if (world.Sprite[10].x < world.Sprite[20].x) {
                    trueorg = world.Sprite[0].x
                        + ((world.Sprite[20].x - world.Sprite[0].x) / 2);
                }
                else {
                    trueorg = world.Sprite[0].x
                        + ((world.Sprite[10].x - world.Sprite[0].x) / 2);
                }
            }

            if (world.Sprite[10].x < world.Sprite[20].x
                && world.Sprite[10].x < world.Sprite[0].x) {
                if (world.Sprite[0].x < world.Sprite[20].x) {
                    trueorg = world.Sprite[10].x
                        + ((world.Sprite[20].x - world.Sprite[10].x) / 2);
                }
                else {
                    trueorg = world.Sprite[10].x
                        + ((world.Sprite[0].x - world.Sprite[10].x) / 2);
                }
            }

            if (world.Sprite[20].x < world.Sprite[10].x
                && world.Sprite[20].x < world.Sprite[0].x) {
                if (world.Sprite[10].x < world.Sprite[0].x) {
                    trueorg = world.Sprite[20].x
                        + ((world.Sprite[0].x - world.Sprite[20].x) / 2);
                }
                else {
                    trueorg = world.Sprite[20].x
                        + ((world.Sprite[10].x - world.Sprite[20].x) / 2);
                }
            }

        }

        //rem End of 3 player scrolling

        if (world.gotFocus == -2) {
            if (world.Sprite[j].x < world.Sprite[k].x) {
                trueorg = world.Sprite[k].x
                    + ((world.Sprite[j].x - world.Sprite[k].x) / 2);
            }
            else {
                trueorg = world.Sprite[j].x
                    + ((world.Sprite[k].x - world.Sprite[j].x) / 2);
            }
            if (world.Sprite[j].y < world.Sprite[k].y) {
                penguin = world.Sprite[k].y
                    + ((world.Sprite[j].y - world.Sprite[k].y) / 2);
            }
            else {
                penguin = world.Sprite[j].y
                    + ((world.Sprite[k].y - world.Sprite[j].y) / 2);
            }
            k = 0;
        }

        if (world.gotFocus > -1) {
            trueorg = world.Sprite[world.gotFocus].x;
            penguin = world.Sprite[world.gotFocus].y;
            k = world.gotFocus;
        }

        //if gotFocus <> -1 Then
        //CameraX = (Sprite(gotFocus).x + (Sprite(gotFocus).wide * 0.5)) - 320
        //if CameraX < 1 Then CameraX = 1
        //if CameraX + CameraWidth >= cameraStopX Then CameraX = cameraStopX - 1 - CameraWidth
        //CameraY = (Sprite(gotFocus).y + (Sprite(gotFocus).high * 0.5)) - 240
        //if CameraY < 1 Then CameraY = 1
        //if CameraY + CameraHeight >= cameraStopY Then CameraY = cameraStopY - 1 - CameraHeight
        //End if

        if (world.gotFocus != -1) {
            world.CameraX = (trueorg + (world.Sprite[k].wide * 0.5)) - 320;
            if (world.CameraX < 1) { world.CameraX = 1; }
            if (world.CameraX + world.CameraWidth >= world.cameraStopX) {
                world.CameraX = world.cameraStopX - 1 - world.CameraWidth;
            }
            world.CameraY = (penguin + (world.Sprite[k].high * 0.5)) - 240;
            if (world.CameraY < 1) { world.CameraY = 1; }
            if (world.CameraY + world.CameraHeight >= world.cameraStopY) {
                world.CameraY = world.cameraStopY - 1 - world.CameraHeight;
            }
        }



        //-----------------------------------------------------------
        //START OF NORMAL ROUTINE
        //------------------------------------------------------------







        //Rem-FLICKER-
        for (j = 0; j < world.spritesInUse; ++j) {

            if (boost::starts_with(world.screen, "Level")) {
                int sopoer;
                //TSNOW: This may be wrong- old code was:
                //       sopoer = Val(right(left(screen, 6), 1))
                sopoer = boost::lexical_cast<double>(world.screen.substr(5, 1));
                this->levelR(sopoer, j);
            }

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


        //Rem-------------------------------------------------------------------
        //               THIS SECTION DOES THE JUMPING
        //Rem-------------------------------------------------------------------
        //The ancient key from the past?!!
        //crapple = clock - jumptime(j)
        //z(j) = jumpstart(j) + (jumpstrength(j) * crapple) - (gravity * (crapple ^ 2))
        //If z(j) < 0 Then z(j) = 0: jumptime(j) = 0

        for (j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];

            if (s.jumpTime != 0) {
                s.lastJump = s.z;
                //z=jt+(JS*T*2)-(g*t)*2^2
                if (s.jumpM == 0) { s.jumpM = 1; }
                s.z = s.jumpStart
                    + (
                    (s.jumpStrength * s.jumpM)
                        * ((world.clock - s.jumpTime) * 2)
                        )
                    - (
                        world.Gravity
                        * std::pow(((world.clock - s.jumpTime) * 2), 2)
                        );
                //
                if (s.z < 0) { s.z = 0; s.jumpTime = 0; s.jumpM = 1; }
            }
            else {

                //REM------------------------------------------------------
                //  THis is the added gravity that pulls them down   if the're in the ares.
                if (s.z > 0) { s.z = s.z - world.sFactor; }
            }
        }



        for (j = 0; j < world.spritesInUse; ++j) {
            auto & s = world.Sprite[j];
            //Rem-If s.time > clock Then GoTo gotDog
            //.time = clock + .speed
            //Rem---------------------------------------------------------------
            //'               THIS SECTION UPDATES THE DAVID SPRITE
            //Rem---------------------------------------------------------------

            if (s.name == "Thomas" || s.name == "Nicky" || s.name == "Nick") {
                penguin = 0;
                if (j == 0) { penguin = 0; }
                if (j == 10) { penguin = 1; }
                if (j == 20) { penguin = 2; }

                if (world.player_data[penguin].upKey == true) {
                    if (s.dir != "u") { s.dir = "u"; s.frame = 6; }
                    s.y = s.y - world.sFactor;
                    //s.Frame = s.Frame + 1: if s.Frame > 6 Then s.Frame = 4
                    s.speed = 0; //0.00001
                    if (s.y < world.CameraY) { s.y = world.CameraY; }
                }
                if (world.player_data[penguin].DownKEY == true) {
                    if (s.dir != "d") { s.dir = "d"; s.frame = 10; }
                    s.y = s.y + world.sFactor;
                    //s.Frame = s.Frame + 1: if s.Frame > 9 Then s.Frame = 7
                    s.speed = 0; //0.00001
                    if (s.y > world.CameraY + world.CameraHeight - s.high) {
                        s.y = world.CameraY + world.CameraHeight - s.high;
                    }
                }
                if (world.player_data[penguin].LeftKEY == true) {
                    if (s.dir != "l" && world.player_data[penguin].upKey == false
                        && world.player_data[penguin].DownKEY == false) {
                        s.dir = "l";
                        s.frame = 14;
                    }
                    s.x = s.x - world.sFactor;
                    //s.Frame = s.Frame + 1: if s.Frame > 12 Then s.Frame = 10
                    s.speed = 0;  //0.00001
                    if (s.x < world.CameraX) { s.x = world.CameraX; }
                }
                if (world.player_data[penguin].RightKEY == true) {
                    if (s.dir != "r" && world.player_data[penguin].upKey == false
                        && world.player_data[penguin].DownKEY == false) {
                        s.dir = "r";
                        s.frame = 2;
                    }
                    s.x = s.x + world.sFactor;
                    //s.Frame = s.Frame + 1: if s.Frame > 3 Then s.Frame = 1
                    s.speed = 0;  //0s.00001

                    if (s.x > world.CameraX + world.CameraWidth - s.wide) {
                        s.x = world.CameraX + world.CameraWidth - s.wide;
                    }
                }


                if (s.z == 0) { s.multiJump = 0; }

                if (s.name == "Nicky" && world.player_data[penguin].JumpKey == true
                    && s.multiJump < 3) {
                    world.player_data[penguin].JumpKey = false;
                    //If .z = 0 Then .multiJump = 0
                    s.multiJump = s.multiJump + 1;
                    s.jumpStart = s.z;
                    s.jumpTime = world.clock;
                }


                if (world.player_data[penguin].JumpKey == true && s.z == 0) {

                    s.jumpStart = s.z;
                    s.jumpTime = world.clock;
                }

                //Check for a lack of movement
                if (world.player_data[j / 10].weapon == "bomb") {
                    if (world.player_data[penguin].AttackKey == true
                        && s.miscTime < world.clock) {
                        for (k = j + 1; k < j + 10; ++k) {
                            if (world.Sprite[k].name == "reserved"
                                || world.Sprite[k].name == "") {
                                break;
                            }
                            if (k == j + 9) {
                                k = j + 10;
                                break;
                            }
                        }
                        if (k != j + 10) {
                            world.Sprite[k].speed = 0; //0.00001
                            world.Sprite[k].name = "bomb";
                            world.Sprite[k].x = s.x;
                            world.Sprite[k].y = s.y;
                            world.Sprite[k].z = s.z; //- (Sprite(0).length);
                            world.Sprite[k].wide = 30 * (world.player_data[j / 10].GradeUp + 1);
                            world.Sprite[k].high = 30 * (world.player_data[j / 10].GradeUp + 1);
                            world.Sprite[k].jumpStart = s.jumpStart;
                            world.Sprite[k].jumpStrength = s.jumpStrength;
                            world.Sprite[k].jumpTime = s.jumpTime;
                            world.Sprite[k].length = 15;
                            world.Sprite[k].texture = world.Sprite[j].texture;
                            world.Sprite[k].visible = true;
                            world.Sprite[k].frame = 1;
                            world.Sprite[k].trueVisible = 1;
                            world.Sprite[k].kind = 0;
                            world.Sprite[k].mode = "";
                            world.Sprite[k].miscTime = world.clock + 3;
                            world.Sprite[k].parent = j;
                            //2017: This file doesn't work:
                            // sound.PlaySound("set bomb");
                            //LoadSound k, "fireball.wav"
                            //PlaySound "fireball"
                            s.miscTime = world.clock + 0.25;
                        }
                    }
                } //Nicky Bomb
 //Thomas Fire
                if (world.player_data[j / 10].weapon == "fireball"
                    && world.player_data[j / 10].ThreeWay == false) {
                    if (world.player_data[penguin].AttackKey == true
                        && s.miscTime < world.clock) {
                        for (k = j + 1; k <= j + 9; ++ k) {
                            if (world.Sprite[k].name == "reserved"
                                || world.Sprite[k].name == "") {
                                break;
                            }
                            if (k == j + 9) {
                                k = j + 10;
                                break;
                            }
                        }
                        if (k == j + 10) {
                            goto outofammo;
                        }
                        if (s.dir == "u") {
                            world.Sprite[k].seekx = s.x;
                            world.Sprite[k].seeky =
                                s.y - (world.CameraHeight * 2);
                            world.Sprite[k].dir = "u";
                        }
                        if (s.dir == "d") {
                            world.Sprite[k].seekx = s.x;
                            world.Sprite[k].seeky =
                                s.y + (2 * world.CameraHeight);
                            world.Sprite[k].dir = "d";
                        }
                        if (s.dir == "l") {
                            world.Sprite[k].seeky = s.y;
                            world.Sprite[k].seekx
                                = s.x - (world.CameraWidth * 2);
                            world.Sprite[k].dir = "l";
                        }
                        if (s.dir == "r") {
                            world.Sprite[k].seeky = s.y;
                            world.Sprite[k].seekx
                                = s.x + (world.CameraWidth * 2);
                            world.Sprite[k].dir = "r";
                        }
                        if (world.player_data[penguin].RightKEY == true) {
                            world.Sprite[k].seekx
                                = s.x + (2 * world.CameraWidth);
                        }
                        if (world.player_data[penguin].LeftKEY == true) {
                            world.Sprite[k].seekx
                                = s.x - (world.CameraWidth * 2);
                        }
                        if (world.player_data[penguin].upKey == true) {
                            world.Sprite[k].seeky
                                = s.y - (world.CameraHeight * 2);
                        }
                        if (world.player_data[penguin].DownKEY == true) {
                            world.Sprite[k].seeky
                                = s.y + (world.CameraHeight * 2);
                        }
                        if (s.mode == "truck") {
                            world.Sprite[k].seeky
                                = world.CameraY - world.CameraHeight;
                            world.Sprite[k].seekx = s.x;
                            world.Sprite[k].dir = "u";
                        }
                        //Sprite(1).visible = true
                        world.Sprite[k].speed = 0; //0.00001
                        world.Sprite[k].name = "fireball";
                        world.Sprite[k].mph = 3;
                        world.Sprite[k].x = s.x;
                        world.Sprite[k].y = s.y;
                        world.Sprite[k].z = s.z; //- (Sprite(0).length)
                        world.Sprite[k].wide = 30 * (world.player_data[j / 10].GradeUp + 1);
                        world.Sprite[k].high = 30 * (world.player_data[j / 10].GradeUp + 1);
                        world.Sprite[k].length = 15;
                        world.Sprite[k].texture = world.Sprite[j].texture;
                        world.Sprite[k].visible = true;
                        world.Sprite[k].kind = 3;
                        //Sprite[k].soundFile = "fireball.wav"
                        world.Sprite[k].parent = j;
                        //LoadSound k, "fireball.wav"
                        if (world.player_data[world.Sprite[k].parent / 10].playerName == "Thomas") {
                            sound.PlaySound("fireball");
                        }
                        if (world.player_data[world.Sprite[k].parent / 10].playerName == "Nick") {
                            sound.PlaySound("iceshot");
                        }

                        s.miscTime = world.clock + 0.25;
                outofammo:
                        EMPTY_LABEL_HACK
                    }
                } //if thomas if

                if (world.player_data[j / 10].weapon == "fireball"
                    && world.player_data[j / 10].ThreeWay == true) {
                    if (world.player_data[penguin].AttackKey == true
                        && s.miscTime < world.clock) {
                        for (k = j + 1; k <= j + 6; ++k) {
                            if (world.Sprite[k].name == "reserved"
                                || world.Sprite[k].name == "") {
                                break;
                            }
                            if (k == j + 9) {
                                k = j + 10;
                                break;
                            }
                        }
                        if (k == j + 10) {
                            goto outofammo3;
                        }
                        if (s.dir == "l") {
                            world.Sprite[k].seeky = s.y;
                            world.Sprite[k].seekx
                                = s.x - (world.CameraWidth * 2);
                            world.Sprite[k].dir = "l";
                            world.Sprite[k + 1].seekx
                                = s.x - (world.CameraWidth * 2);
                            world.Sprite[k + 1].seeky
                                = s.y + (world.CameraHeight * 2);
                            world.Sprite[k + 1].dir = "l";
                            world.Sprite[k + 2].seekx
                                = s.x - (world.CameraWidth * 2);
                            world.Sprite[k + 2].seeky
                                = s.y - (world.CameraHeight * 2);
                            world.Sprite[k + 2].dir = "l";
                        }
                        if (s.dir == "r") {
                            world.Sprite[k].seeky = s.y;
                            world.Sprite[k].seekx
                                = s.x + (world.CameraWidth * 2);
                            world.Sprite[k].dir = "r";
                            world.Sprite[k + 1].seekx
                                = s.x + (world.CameraWidth * 2);
                            world.Sprite[k + 1].seeky
                                = s.y + (world.CameraHeight * 2);
                            world.Sprite[k + 1].dir = "r";
                            world.Sprite[k + 2].seekx
                                = s.x + (world.CameraWidth * 2);
                            world.Sprite[k + 2].seeky
                                = s.y - (world.CameraHeight * 2);
                            world.Sprite[k + 2].dir = "r";
                        }
                        if (world.player_data[penguin].upKey == true || s.dir == "u") {
                            world.Sprite[k].seekx = s.x;
                            world.Sprite[k].seeky = s.y - (world.CameraHeight * 2);
                            world.Sprite[k].dir = "u";
                            world.Sprite[k + 1].seekx
                                = s.x - (world.CameraWidth * 2);
                            world.Sprite[k + 1].seeky
                                = s.y - (world.CameraHeight * 2);
                            world.Sprite[k + 1].dir = "u";
                            world.Sprite[k + 2].seekx
                                = s.x + (world.CameraWidth * 2);
                            world.Sprite[k + 2].seeky
                                = s.y - (world.CameraHeight * 2);
                            world.Sprite[k + 2].dir = "u";
                            if (world.player_data[penguin].LeftKEY == true) {
                                world.Sprite[k + 2].seeky = s.y;
                                world.Sprite[k + 2].seekx
                                    = s.x - (world.CameraWidth * 2);
                                world.Sprite[k + 2].dir = "l";
                            }
                            if (world.player_data[penguin].RightKEY == true) {
                                world.Sprite[k + 1].seeky = s.y;
                                world.Sprite[k + 1].seekx
                                    = s.x + (world.CameraWidth * 2);
                                world.Sprite[k + 1].dir = "r";
                            }
                        }
                        if (world.player_data[penguin].DownKEY == true
                            || s.dir == "d") {
                            world.Sprite[k].seekx = s.x;
                            world.Sprite[k].seeky = s.y + (2 * world.CameraHeight);
                            world.Sprite[k].dir = "d";
                            world.Sprite[k + 1].seekx = s.x - (world.CameraWidth * 2);
                            world.Sprite[k + 1].seeky = s.y + (world.CameraHeight * 2);
                            world.Sprite[k + 1].dir = "d";
                            world.Sprite[k + 2].seekx = s.x + (world.CameraWidth * 2);
                            world.Sprite[k + 2].seeky = s.y + (world.CameraHeight * 2);
                            world.Sprite[k + 2].dir = "d";
                            if (world.player_data[penguin].LeftKEY == true) {
                                world.Sprite[k + 2].seeky = s.y;
                                world.Sprite[k + 2].seekx
                                    = s.x - (world.CameraWidth * 2);
                                world.Sprite[k + 2].dir = "l";
                            }
                            if (world.player_data[penguin].RightKEY == true) {
                                world.Sprite[k + 1].seeky = s.y;
                                world.Sprite[k + 1].seekx
                                    = s.x + (world.CameraWidth * 2);
                                world.Sprite[k + 1].dir = "r";
                            }
                        }
if (s.mode == "truck") {
                            world.Sprite[k].seeky
                                = world.CameraY - world.CameraHeight;
                                world.Sprite[k].seekx = s.x;
                                world.Sprite[k].dir = "u";
                        }
                        //Sprite(1).visible = True
                        for (trueorg = k; trueorg <= k + 2; ++ trueorg) {
                            world.Sprite[trueorg].speed = 0;  //0.00001
                            world.Sprite[trueorg].name = "fireball";
                            world.Sprite[trueorg].mph = 3;
                            world.Sprite[trueorg].x = s.x;
                            world.Sprite[trueorg].y = s.y;
                            world.Sprite[trueorg].z = s.z; //- (world.Sprite[0).lengh)
                            world.Sprite[trueorg].wide
                                = 30 * (world.player_data[j / 10].GradeUp + 1);
                            world.Sprite[trueorg].high
                                = 30 * (world.player_data[j / 10].GradeUp + 1);
                            world.Sprite[trueorg].length = 15;
                            world.Sprite[trueorg].texture
                                = world.Sprite[j].texture;
                            world.Sprite[trueorg].visible = true;
                            world.Sprite[trueorg].kind = 3;
                            world.Sprite[trueorg].frame = 1;
                            world.Sprite[trueorg].soundFile = "fireball.wav";
                            world.Sprite[trueorg].parent = j;
                        }
                        //LoadSound k, "fireball.wav"
                        sound.PlaySound("fireball");
                        s.miscTime = world.clock + 0.25;
                outofammo3:
                        EMPTY_LABEL_HACK
                    }
                } //if thomas if


                if (world.player_data[penguin].upKey == false
                    && world.player_data[penguin].DownKEY == false
                    && world.player_data[penguin].LeftKEY == false
                    && world.player_data[penguin].RightKEY == false) {
                    if (s.dir == "r") { s.frame = 2; }
                    if (s.dir == "l") { s.frame = 14; }
                    if (s.dir == "u") { s.frame = 6; }
                    if (s.dir == "d") { s.frame = 10; }
                    s.speed = 0;
                }

            } //-End of David Sprite If

            //- END OF DAVID SPRITE--------------------------------------------
            //
            //Rem--------------------------------------------------------------


            if (s.name == "goomba") {
                this->seeker(j);

                if (s.x == s.seekx && s.y == s.seeky) {
                    k = (int)(random.next() * 4) + 1;
                    s.seekx = s.x;
                    s.seeky = s.y;
                    if (k == 1) { s.seekx = s.x + 25; }
                    if (k == 2) { s.seekx = s.x - 25; }
                    if (k == 3) { s.seeky = s.y + 25; }
                    if (k == 4) { s.seeky = s.y - 25; }
                    if ((s.seekx + s.wide) > 640) { s.seekx = 640 - s.wide; }
                    if (s.seekx < 1) { s.seekx = 0; }
                    if ((s.seeky + s.high) > 480) { s.seeky = 480 - s.high; }
                    if (s.seeky < 1) { s.seeky = 0; }
                }
            }

            if (s.name == "Kerbose Death") {
                //.flickerTime = clock + 2
                //if (.color = QBColor(1) Then .color = normColor Else .color = QBColor(1)
                if (s.miscTime < world.clock) {
                    s.name = "";
                    s.visible = false;
                    s.trueVisible = 2;
                    s.flickerTime = world.clock + 1;
                }
            }

            if (s.name == "Kerbose") {
                k = (int) (random.next() * 2) + 1;
                if (k == 1) { s.x = s.x + world.sFactor; }
                if (k == 2) { s.x = s.x - world.sFactor; }
                k = (int) (random.next() * 2) + 1;
                if (k == 1) { s.y = s.y + world.sFactor; }
                if (k == 2) { s.y = s.y - world.sFactor; }
                k = (int) (random.next() * 20) + 1;
                if (k == 1) {
                    if (s.z == 0) {
                        s.jumpStart = s.z;
                        s.jumpTime = world.clock;
                    }
                }
            }



            if (s.name == "fireball") {

                if (world.player_data[s.parent / 10].slicer == true) {
                    if (s.color == view.QBColor(2)) {
                        s.color = view.QBColor(15);
                    }
                    if (s.color == view.QBColor(7)) { s.color = view.QBColor(2); }
                    if (s.color == view.QBColor(10)) { s.color = view.QBColor(7); }
                    if (s.color == view.QBColor(14)) {s.color = view.QBColor(10); }
                    if (s.color == view.QBColor(1)) {s.color = view.QBColor(14); }
                    if (s.color == view.QBColor(4)) { s.color = view.QBColor(1); }
                    if (s.color == view.QBColor(15)) { s.color = view.QBColor(4); }

                    if ((int) (random.next() * 1) == 1) {
                        s.jumpStrength = 75;
                        s.maxJump = 2;
                        this->makeJump(j);
                    }

                }


                s.frame = s.frame + 1;
                if (s.dir == "r") {
                    if (s.frame > 3) { s.frame = 1; }
                }
                if (s.dir == "l") {
                    if (s.frame > 6) { s.frame = 4; }
                }
                if (s.dir == "u") {
                    if (s.frame > 9) { s.frame = 7; }
                }
                if (s.dir == "d") {
                    if (s.frame > 12) { s.frame = 10; }
                }
                //Rem----------------------
                s.flickerTime = world.clock + 1;
                if (s.x < s.seekx) { s.x = s.x + (s.mph * world.sFactor); }
                if (s.x > s.seekx) { s.x = s.x - (s.mph * world.sFactor); }
                if (s.y < s.seeky) { s.y = s.y + (s.mph * world.sFactor); }
                if (s.y > s.seeky) { s.y = s.y - (s.mph * world.sFactor); }
                //.wide = .wide + 1
                //.high = .high + 1
                //if (.x = .seekx And .y = .seeky Or (.x + .wide) < 1 Or .x > 640 Or (.y + .high) < 1 Or .y > 480 Then .visible = False: .name = "reserved"
                //if (.x >= .seekx And .x <= (.seekx + .mph) And .y >= .seeky And .y <= (.seeky + .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
                //if (.x <= .seekx And .x >= (.seekx - .mph) And .y <= .seeky And .y >= (.seeky - .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
                if (s.x > (world.CameraX + world.CameraWidth)
                    || s.x < (world.CameraX - world.CameraWidth)
                    || s.y > (world.CameraY + world.CameraHeight)
                    || s.y < (world.CameraY - world.CameraHeight)) {
                    s.visible = false;
                    s.flickerTime = 0;
                    s.name = "reserved";
                    s.trueVisible = 2;
                }

                this->offCameraKill(j);
            }

            if (s.name == "bomb") {
                s.frame = s.frame + 1;
                if (s.mode != "explode") {
                    if (s.frame > 3) { s.frame = 2; }
                }
                if (s.mode == "explode") {
                    if (s.frame > 5) { s.frame = 4; }
                }
                if (s.kind == 3 && s.miscTime < world.clock) {
                    s.visible = false;
                    s.flickerTime = 0;
                    s.name = "reserved";
                    s.trueVisible = 2;
                    s.kind = 0;
                    goto fuddle;
                }
                if (s.kind == 3) {
                    goto fuddle;
                }
                if (s.kind == 0 && s.miscTime > world.clock) {
                    goto fuddle;
                }
                s.kind = 3;
                s.miscTime = world.clock + 1;
                s.mode = "explode";
                sound.PlaySound("bomb explode");
                s.flickerTime = world.clock + 1;
            fuddle:
                EMPTY_LABEL_HACK
            }


            if (s.name == "falling") {
                //.flickerTime = clock + 1
                s.z = s.z - world.sFactor;
                if (s.z < 1) {
                    s.z = 1;
                    s.name = "deceased";
                    s.visible = false;
                    s.trueVisible = 2;
                    s.flickerTime = 0;
                }
                s.frame = s.frame + 1;
                if (s.frame > 4) { s.frame = 3; }
            }

            if (s.name == "Dying Explosion") {
                if (s.wide < s.seekx) {
                    s.wide = s.wide + s.mph;
                    s.x = s.x - (s.mph * 0.5 * world.sFactor);
                    s.high = s.high + s.mph;
                    s.y = s.y - (s.mph * 0.5 * world.sFactor);
                }
                if (s.wide >= s.seekx) {
                    s.high = s.high - s.mph;
                    s.y = s.y + (s.mph * 0.5 * world.sFactor);
                    if (s.high < 1) {
                        s.name = "deceased";
                        s.visible = false;
                    }
                }
            }

            if (s.name == "Death of David") {
                //if (.seekx = 0) then .seekx = .x: .seeky = .y
                //if (.color = QBColor(4)) then .color = QBColor(1) Else .color = QBColor(4)
                s.flickerTime = world.clock + 1;
                if (s.wide < s.seekx) {
                    s.wide = s.wide + s.mph * 3; //instead of * 3, it used to be * 0.5
                    s.high = s.high + s.mph * 3;
                }
                if (s.wide >= s.seekx) {
                    s.high = s.high - s.mph * 3;
                    if (s.high < (-1 * s.high)) {
                        //what to do if dead
                        s.name = "deceased";
                        s.visible = false;
                        s.name = "dead";
                        s.srcx = 2;
                        s.srcy = 363;
                        s.srcx2 = 96;
                        s.srcy2 = 379;
                        s.texture = 0;
                        s.visible = true;
                        world.player_data[j / 10].lives = world.player_data[j / 10].lives - 1;
                        if (world.player_data[j / 10].lives < 1) {
                            if (world.continues > 0) {
                                s.name = "continue";
                                s.texture = 0;
                                s.srcx = 3;
                                s.srcy = 345;
                                s.srcx2 = 96;
                                s.srcy2 = 361;
                                s.wide = 93; //16;
                                s.high = 16; //93;
                                s.frame = 0;
                                world.Sprite[j + 1].name = "continue number";
                                world.Sprite[j + 1].color = normColor;
                                world.Sprite[j + 1].frame = 11;
                                world.Sprite[j + 1].texture = 0;
                                world.Sprite[j + 1].miscTime = world.clock + 2;
                                this->loadAnimation(j + 1, "continue.ani");

                                s.kind = 0;
                                world.Sprite[j + 1].kind = 0;
                                world.Sprite[j + 1].visible = true;
                                world.Sprite[j + 1].wide = 20;
                                world.Sprite[j + 1].high = 20;
                                s.y = 10;
                                world.Sprite[j + 1].y = s.y;
                                s.y = 10;
                                world.Sprite[j + 1].x = s.x + 100;
                            } //end continue if
                        } //end lives if
                        if (world.player_data[j / 10].lives > 0) {
                            this->createPlayer(j);
                            world.Sprite[j].name = world.player_data[j / 10].playerName;
                            world.Sprite[j].kind = 1;
                            this->initSprites(j);
                            world.Sprite[j].flickerTime = world.clock + 5;
                            //Sprite[j].x = .seekx: .seekx = 0
                            //Sprite[j].y = .seekx: .seekx = 0

                        }

                    }
                }
            }

            if (s.name == "continue") {
                if (this->anyKey(j / 10) == 1) {
                    world.continues = world.continues - 1;
                    world.player_data[j / 10].lives = 2;
                    this->createPlayer(j);
                    world.Sprite[j].name = world.player_data[j / 10].playerName;
                    world.Sprite[j].kind = 1;
                    world.Sprite[j].flickerTime = world.clock + 5;
                    this->initSprites(j);
                }
                s.visible = true;
                if ((j / 10) == 0) {
                    s.x = world.CameraX + 10;
                }
                if ((j / 10) == 1) {
                    s.x = world.CameraX + 250;
                    s.color = view.QBColor(10);
                }
                if ((j / 10) == 2) {
                    s.x = world.CameraX + 450;
                    s.color = view.QBColor(14);
                }
                s.y = world.CameraY + 10;
            }

            if (s.name == "continue number") {
                s.trueVisible = 1;
                if (((j - 1) / 10) == 0) {
                    s.x = world.CameraX + 10 + 93;
                }
                if (((j - 1) / 10) == 1) {
                    s.x = world.CameraX + 250 + 93;
                }
                if (((j - 1) / 10) == 2) {
                    s.x = world.CameraX + 450 + 93;
                }
                s.y = world.CameraY + 10;

                if (s.miscTime < world.clock) {
                    s.miscTime = world.clock + 2;
                    s.frame = s.frame - 1;
                    if (s.frame == 0) { s.frame = 13; }
                    s.visible = true;
                    if (world.continues < 1) {
                        s.frame = 12; //this is useful, say, if two people have the choice of the last continue and one gets it before someone else
                    }
                    if (s.frame == 10) { sound.PlayWave("ConTen.wav"); }
                    if (s.frame == 9) { sound.PlayWave("ConNine.wav"); }
                    if (s.frame == 8) { sound.PlayWave("ConEight.wav"); }
                    if (s.frame == 7) { sound.PlayWave("ConSeven.wav"); }
                    if (s.frame == 6) { sound.PlayWave("ConSix.wav"); }
                    if (s.frame == 5) { sound.PlayWave("ConFive.wav"); }
                    if (s.frame == 4) { sound.PlayWave("ConFour.wav"); }
                    if (s.frame == 3) { sound.PlayWave("ConThree.wav"); }
                    if (s.frame == 2) { sound.PlayWave("ConTwo.wav"); }
                    if (s.frame == 1) { sound.PlayWave("ConOne.wav"); }
                    if (s.frame == 13) { sound.PlayWave("ConZero.wav"); }
                    if (s.frame == 12) {
                        world.Sprite[j - 1].name = "dead";
                        world.Sprite[j - 1].visible = false;
                        this->killS(j); //": .visible = False
                    }
                }
            }

            if (s.name == "GameOverCloudTitle") {
                if (world.clock > s.miscTime) {
                    s.high = s.high + 2 * world.sFactor;
                    s.y = s.y - world.sFactor;
                    if (s.y < 0) { s.flickerTime = world.clock + 1; }
                    if (s.y < -300) {
                        s.wide = s.wide - (10 * world.sFactor);
                        s.x = s.x + 5 * world.sFactor;
                        if (s.wide < 0) {
                            s.visible = false;
                            world.screen = "title";
                        }
                    }
                }
            }

            if (s.name == "Selecter") {
                if (world.clock > s.miscTime) {
                    s.miscTime = world.clock + 0.1;  //this way, the person won't scream through the selection box because it moves at 40 FPS!
                    //For k = 0 To 2
                    k = j;
                    if (s.visible == false) {
                        if (this->anyKey(k) == 1) {
                            s.visible = true;
                            s.frame = k + 1;
                        }
                    }

                    if (world.player_data[k].upKey == true) {
                        s.frame = s.frame - 1;
                        if (s.frame == 0) { s.frame = 5; }
                    }
                    if (world.player_data[k].DownKEY == true) {
                        s.frame = s.frame + 1;
                        if (s.frame == 6) { s.frame = 1; }
                    }
                    if (world.player_data[k].AttackKey == true
                        || world.player_data[k].JumpKey == true) {
                        s.mode = "done";
                        s.name = "KukoFax";
                        if (s.frame == 1) { sound.PlayWave("PickTom.wav"); }
                        if (s.frame == 2) { sound.PlayWave("PickNick.wav"); }
                        if (s.frame == 3) { sound.PlayWave("PickDrew.wav"); }
                        if (s.frame == 5) { sound.PlayWave("PickNicky.wav"); }
                    }
                //Next k
                }

            }

            if (s.name == "StageWhat") {
                //
                if (s.mode == "") {
                    //playWave "conten.wav"
                    if (s.miscTime == 0) {
                        s.miscTime = world.clock + 2;
                        world.Sprite[31].zOrder = -100;
                        this->findOrder();
                    }

                    {
                        //TSNOW: No idea why this is hardcoded.
                        auto & s2 = world.Sprite[31];
                        if (world.Sprite[j].miscTime > world.clock) {
                            s2.x = world.CameraX;
                            s2.y = world.CameraY;
                            s2.wide = 640;
                            s2.high = 480;
                            s2.frame = 9;
                            s2.texture = 9;
                            s2.visible = true;
                        }

                        if (world.Sprite[j].miscTime < world.clock) {
                            s2.x = s2.x + 4 * world.sFactor;
                            s2.y = s2.y + 4 * world.sFactor;
                            s2.wide = s2.wide - 8 * world.sFactor;
                            s2.high = s2.high - 8 * world.sFactor;
                            s2.zOrder = -100;
                            if (s2.wide < 0) {
                                world.Sprite[j].mode = "?";
                                world.Sprite[30].miscTime = world.clock + 3;
                            }
                        }
                    }  // End With
                }  //mode
                if (s.mode == "?") {
                    //playWave "conten.wav"
                    {
                        auto & s2 = world.Sprite[31];
                        unstretch(s2);
                        s2.wide = s2.wide * 2;
                        s2.high = s2.high * 2;

                        //.wide = (275 - 188) * 2
                        //.high = (376 - 353) * 2

                        s2.frame = 10;
                        s2.texture = 9;
                        s2.visible = true;
                        s2.zOrder = -100;
                        this->centerSprite(31);
                    }
                    if (s.miscTime < world.clock) {
                        s.mode = "2";
                        s.name = "script";
                        this->findOrder();
                    }
                }


            }

            if (s.name == "script") {
                this->script();
            }


            //playWave "conten.wav"

            if (s.name == "frontdoor") {
                if (this->findQ("Kerbose") < 1) {
                    this->killS(j);
                    s.name = "exit";
                    this->cinemaM(2);
                }
            }

            if (s.name == "exit") {
                //TSNOW: This looks to step up by 10 so should only iterate
                //       the loop once, but that's what the old code did.
                for (penguin = 0; penguin <= 2; penguin+= 10) {
                    if (this->hitdetection(j, penguin) != 0
                        && world.Sprite[penguin].name
                        == world.player_data[(penguin / 10)].playerName) {
                        world.exitS = "true";
                    }
                }
            }

            if (s.name == "tdigger") {
                if (s.mode == "") {
                    unstretch(world.Sprite[j]);
                    //TSNOW: Another funky step 10 loop.
                    for (penguin = 0; penguin <= 2; penguin += 10) {
                        if (this->hitdetection(j, penguin, true) == 5
                            && world.Sprite[penguin].name
                            == world.player_data[(penguin / 10)].playerName) {
                            s.mode = "runner";
                            s.seekx = world.cameraStopX;
                            //.mhp = 10
                            s.kind = 2;
                            s.deathType = "expand";
                            //.hp = 1
                            s.reverse = true;
                        }
                    }
                }

                if (s.target != 0 || s.target == -1) {
                    s.target = this->checkProx(j);
                    s.seekx = this->getMiddleX(s.target);
                    s.seeky = this->getMiddleY(s.target);
                }

                if (s.mode == "runner") {
                    if (this->getProx(j, s.target) > 50) {
                        s.seekx = this->getMiddleX(s.target);
                    }
                    if (this->getProx(j, s.target) < 50) {
                        s.mph = s.mph - 1;
                        if (s.mph < -90) {
                            s.seekx = this->getMiddleX(s.target);
                            s.mph = 10;
                        }
                    }
                    s.seeky = this->getMiddleY(s.target);
                    if (this->getProx(j, s.target) < 100) {
                        s.mph = (this->getProx(j, s.target)) / 100;
                    } else {
                        s.mph = 10;
                    }
                    if (this->getProx(j, s.target) == 0) {
                        s.seekx = world.Sprite[s.target].x;
                        s.seeky = world.Sprite[s.target].y;
                    }

                    this->seeker(j);

                    //Stop
                    //if (sgetMiddleX(.target) > getMiddleX(j)) then .x = .x + (sFactor * 3)
                    //if (sgetMiddleX(.target) < getMiddleX(j)) then .x = .x - (sFactor * 3)

                    //if (sgetMiddleY(.target) > getMiddleY(j)) then .y = .y + (sFactor * 1)
                    //if (sgetMiddleY(.target) < getMiddleY(j)) then .y = .y - (sFactor * 1)
                }

            }


            if (s.name == "expand") {
                s.kind = 0;
                //if (s.mode = "runner") then
                s.frame = 3;
                s.SpriteVerts[0].rhw
                    = s.SpriteVerts[0].rhw + (0.01 * world.sFactor);
                s.SpriteVerts[3].rhw
                    = s.SpriteVerts[3].rhw + (0.01 * world.sFactor);
                s.wide = s.wide + (world.sFactor);
                s.x = s.x - (world.sFactor / 2);
                s.high = s.high + (world.sFactor);
                s.y = s.y - (world.sFactor / 2);
                if (s.SpriteVerts[3].rhw > 2) {
                    s.name = "harharhar";
                    this->initSprites(j); //: killS j
                }
            }

            if (s.name == "harharhar") {
                if (s.flickerTime < world.clock) {
                    this->killS(j);
                }
            }


            if (s.name == "dead") {
                //Stop
                if ((j / 10) == 0) {
                    s.x = world.CameraX + 10;
                }
                if ((j / 10) == 1) {
                    s.x = world.CameraX + 250;
                    s.color = view.QBColor(10);
                }
                if ((j / 10) == 2) {
                    s.x = world.CameraX + 450;
                    s.color = view.QBColor(14);
                }
                s.y = world.CameraY + 10;
                s.frame = 0;
                s.visible = false;
                s.srcx = 2;
                s.srcy = 363;
                s.srcx2 = 96;
                s.srcy2 = 379;
                s.texture = 0;
                s.visible = true;
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

        for (j = 0; j <= world.spritesInUse; ++ j) {
            //If Sprite(j).mover = True Then
            for (k = j + 1; k <= world.spritesInUse; ++k) {
                if (world.Sprite[j].kind == 0) { goto fthis2; }
                if (world.Sprite[k].kind == world.Sprite[j].kind
                    || world.Sprite[k].kind == 0) {
                    goto fthis2;
                }
                //If Sprite(k).mover = False And Sprite(j).mover = False Then GoTo fthis2
                //If k = 1 And j = 31 Then Stop
                if (this->hitdetection(j, k) != 0) {
                    this->checkHit(j, k);
                }
            fthis2:
                EMPTY_LABEL_HACK
            }

        }

        if (this->flipGame()) {
            // Return early since we just destroyed this object... :/
            return;
        }
        world.gpRate2 = world.gpRate2 + 1;
        if (world.clock > world.cranBerry2) {
            world.cranBerry2 = (world.clock + 1);
            world.gpRate = world.gpRate2;
            world.gpRate2 = 0;
        }
        for (j = 0; j < world.spritesInUse; ++ j) {
            world.Sprite[j].lastX = world.Sprite[j].x;
            world.Sprite[j].lastY = world.Sprite[j].y;
        }

    }
    //     End Sub for playGame--------------------------------------------!!!
    //     End Sub for playGame--------------------------------------------!!!
    //     End Sub for playGame--------------------------------------------!!!
    //     End Sub for playGame--------------------------------------------!!!

private:
    long anyKey(int zed) {
        // Returns true if the player at the given index is pressing any key.
        return ((world.player_data[zed].RightKEY || world.player_data[zed].LeftKEY
                 || world.player_data[zed].upKey || world.player_data[zed].DownKEY
                 || world.player_data[zed].AttackKey) ? 1 : 0);
    }

    void centerSprite(int who) {
        // Aims the camera on the given sprite.
        auto & s = world.Sprite[who];
        s.x = (world.CameraX + (world.CameraWidth / 2)) - (s.wide / 2);
        s.y = (world.CameraY + (world.CameraHeight / 2)) - (s.high / 2);
    }

    void checkHit(int j, int k) {
                bool redo = false;
        int HolderJ = 0;

        auto gosub_hurtj = [this, &HolderJ, &j]() {
            HolderJ = j;
            world.Sprite[HolderJ].flickerTime
                = world.clock + world.Sprite[HolderJ].invTime;
            sound.PlaySound(world.Sprite[HolderJ].soundFile);
        };

        auto gosub_hurtK = [this, &HolderJ, &k]() {
            HolderJ = k;
            world.Sprite[HolderJ].flickerTime
                = world.clock + world.Sprite[HolderJ].invTime;
            sound.PlaySound(world.Sprite[HolderJ].soundFile);
        };

    redo2:
        //If k = 1 And j = 31 Then Stop

        //Rem-KIND DEFINITIONS
        //1 is player
        //2 is enemy
        //3 is fireball
        //4 is goomba thing
        //5 is unmoveable
        //6 is trampoline

        //to do
        //7 is a enemy killable by jumping on it
        //8 is bullet by the enemy (can pass through 5//s)

        //Player hits an enemy
        if (world.Sprite[j].kind == 1 && (world.Sprite[k].kind == 2
            || world.Sprite[k].kind == 8)) {
            if (world.Sprite[j].flickerTime < world.clock) {
                world.Sprite[j].hp = world.Sprite[j].hp - 1;
                sound.PlaySound(world.Sprite[j].soundFile);
                gosub_hurtj();
                if (world.Sprite[j].hp <= 0) {
                    world.Sprite[j].name = world.Sprite[j].deathType;
                    this->initSprites(j);
                }
            }
        }

        //Enemy hit by a fireball
        if ((world.Sprite[j].kind == 2 || world.Sprite[j].kind == 7)
            && world.Sprite[k].kind == 3) {

            if (world.Sprite[j].flickerTime < world.clock) {
                world.Sprite[j].hp = world.Sprite[j].hp - 1;
                gosub_hurtj();
                if (world.Sprite[j].hp <= 0) {
                    world.Sprite[j].name = world.Sprite[j].deathType;
                    this->initSprites(j);
                }
            }
            if (world.Sprite[k].name == "fireball") {
                if (world.player_data[(world.Sprite[k].parent) / 10].slicer == false) {
                    this->killS(k);
                }
            }
        }

        //Player steps on a goomba type thing
        if (world.Sprite[j].kind == 1
            && (world.Sprite[k].kind == 4 || world.Sprite[k].kind == 7)) {
            if (world.Sprite[j].z > world.Sprite[k].length
                && world.Sprite[j].lastJump > world.Sprite[j].z) {
                world.Sprite[j].jumpStart = world.Sprite[j].z;  //sends thing up into the air
                world.Sprite[j].jumpTime = world.clock; //sends thing up into the air, even if (goomba is flashing
                if (world.Sprite[k].flickerTime < world.clock) {
                    world.Sprite[k].hp = world.Sprite[k].hp - 1;
                    gosub_hurtK();
                    sound.PlaySound("spring");
                    world.Sprite[j].jumpM = world.Sprite[k].jumpM;
                    if (world.Sprite[k].hp <= 0) {
                        world.Sprite[k].name = world.Sprite[k].deathType;
                        this->initSprites(k);
                    }
                }
            }
        }

        //Player illigetimately touches goomba.
        if (world.Sprite[j].kind == 1
            && (world.Sprite[k].kind == 4 || world.Sprite[k].kind == 7)) {
            if (world.Sprite[j].flickerTime < world.clock) {
                if (world.Sprite[j].z < world.Sprite[k].length
                    || world.Sprite[j].lastJump < world.Sprite[j].z) {
                    world.Sprite[j].hp = world.Sprite[j].hp - 1;
                    gosub_hurtj();
                    if (world.Sprite[j].hp <= 0) {
                        world.Sprite[j].name = world.Sprite[j].deathType;
                        this->initSprites(j);
                    }
                }
            }
        }


        //Player jumps on bouncy object
        if (world.Sprite[k].kind == 6) {

            if (world.Sprite[j].kind == 3 || world.Sprite[j].kind == 8) {
                goto overalready;
            }

            //BOUNCE TIME!
            if (world.Sprite[j].z > world.Sprite[k].length
                && world.Sprite[j].lastJump > world.Sprite[j].z) {
                world.Sprite[j].jumpStart = world.Sprite[j].z; //sends thing up into the air
                world.Sprite[j].jumpTime = world.clock; //sends thing up into the air, even if (goomba is flashing
                sound.PlaySound("spring");
                world.Sprite[k].mode = "bounce";
                world.Sprite[k].miscTime = world.clock + 1;
                world.Sprite[j].jumpM = world.Sprite[k].jumpM;
            }
            //OH CRAP! I NO BOUNCE
            if (world.Sprite[j].z < world.Sprite[k].length
                || world.Sprite[j].lastJump < world.Sprite[j].z) {
                if (world.Sprite[j].kind == 5 || world.Sprite[j].kind == 3
                    || world.Sprite[j].kind == 6) {
                    goto britneyDog2;
                }
                //if (world.Sprite[k].z > (world.Sprite[j].length - (world.Sprite[j].length * 0.75))) then GoTo britneyDog2
            //2017: this label was unreferneced:
            // poddle2:
                //HolderJ = hitdetection(j, k, True)
                //if (HolderJ = 2 || HolderJ = 6 || HolderJ = 7) then
                //world.Sprite[j].x = world.Sprite[j].x - world.Sprite[j].mph
                //End If
                //if (HolderJ = 3 || HolderJ = 8 || HolderJ = 9) then
                //world.Sprite[j].x = world.Sprite[j].x + world.Sprite[j].mph
                //End If
                //if (HolderJ = 4 || HolderJ = 6 || HolderJ = 8) then
                //world.Sprite[j].y = world.Sprite[j].y - world.Sprite[j].mph
                //End If
                //if (HolderJ = 5 || HolderJ = 7 || HolderJ = 9) then
                //world.Sprite[j].y = world.Sprite[j].y + world.Sprite[j].mph
                //End If
                //if (hitdetection(k, j) = 1) then GoTo poddle2 //
                world.Sprite[j].x = world.Sprite[j].lastX;
                world.Sprite[j].y = world.Sprite[j].lastY;
        britneyDog2:
                EMPTY_LABEL_HACK
            }
        overalready:
            EMPTY_LABEL_HACK
        }


        if (world.Sprite[j].kind == 5) {
            if (world.Sprite[k].kind == 5 || world.Sprite[k].kind == 3
                || world.Sprite[k].kind == 6 || world.Sprite[k].kind == 8) {
                goto britneyDog;
            }
        // 2017: unreferenced label:
        // poddle:
            //HolderJ = hitdetection(k, j, True)
            //if (HolderJ = 2 || HolderJ = 6 || HolderJ = 7) then
            //world.Sprite(k).x = world.Sprite(k).x - world.Sprite(k).mph
            //End If
            //if (HolderJ = 3 || HolderJ = 8 || HolderJ = 9) then
            //world.Sprite(k).x = world.Sprite(k).x + world.Sprite(k).mph
            //End If
            //if (HolderJ = 4 || HolderJ = 6 || HolderJ = 8) then
            //world.Sprite(k).y = world.Sprite(k).y - world.Sprite(k).mph
            //End If
            //if (HolderJ = 5 || HolderJ = 7 || HolderJ = 9) then
            //world.Sprite(k).y = world.Sprite(k).y + world.Sprite(k).mph
            //End If
            //if (hitdetection(k, j) = 1) then GoTo poddle
            world.Sprite[k].x = world.Sprite[k].lastX;
            world.Sprite[k].y = world.Sprite[k].lastY;
        britneyDog:
            EMPTY_LABEL_HACK
        }

        if (redo == false) {
            redo = true; HolderJ = j; j = k; k = HolderJ; goto redo2;
        }
        if (redo == true) { HolderJ = j; j = k; k = HolderJ; }

        return;  // exit sub
    }

    int checkProx(const int who) {
        //numberPlayers integer legend
        //1 Only player 1
        //2 Player 1 and 2
        //3 All three Players
        //4 Just player 2
        //5 Just player 3
        //6 Players 1 and 3
        //7 Players 2 and 3

        int theclosest = 0;
        int min = 0;

        min = 9999;
        theclosest = 0;
        for (int penguin = 0; penguin <= 2; ++ penguin) {
            if (penguin == 0 &&
                (world.numberPlayers == 4 || world.numberPlayers == 5 || world.numberPlayers == 7)) {
                continue;
            }
            if (penguin == 1 &&
                (world.numberPlayers == 1 || world.numberPlayers == 5 || world.numberPlayers == 6)) {
                continue;
            }
            if (penguin == 2 &&
                (world.numberPlayers == 1 || world.numberPlayers == 2 || world.numberPlayers == 4)) {
                continue;
            }

            // abs(x2-x1)^2+abs(y2-y1)^2
            const int buttcat = lp3::narrow<int>(
                std::sqrt(
                    std::pow(
                        std::abs(world.Sprite[penguin * 10].x
                                    - world.Sprite[who].x),
                        2)
                    +
                    std::pow(
                        std::abs(world.Sprite[penguin * 10].y
                                    - world.Sprite[who].y),
                        2)
                ));

            if (buttcat < min) {
                theclosest = penguin;
                min = buttcat;
            }
        }

        return theclosest * 10;
    }

    void cinemaM(int what) {
        //Everyone raise your hand who remembers cinemaM?
        //Yes this classic far too long subroutine has made a triumphant return from Lady Pousha Quest.
        if (what == 2) {
            world.Sprite[30].name = "script";
            world.Sprite[30].mode = "2";
            world.cinemaMax = 4;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1B1.wav", 2.24);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1B2.wav", 2.74);
            this->setCinema(2, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1B3.wav", 4.24);
            this->setCinema(3, 5, 3, 7, 16, 15, 15, 15, 8, "TalkLv1B4.wav", 0.95);
        }
    }

    void createPlayer(int who) {
            int goatorg = 0;

        if (world.player_data[(who / 10)].playerName == "Thomas") {
            world.player_data[who / 10].weapon = "fireball";
            this->loadAnimation(who, "Thomas.ani");
            view.LoadTexture((who / 10) + 1, "Flip1.png", 254, 254);
            world.Sprite[who].texture = (who / 10) + 1;
            for (goatorg = (who + 1); goatorg <= (who + 9); ++ goatorg) {
                this->loadAnimation(goatorg, "Fireball.ani");
            }
        }

        if (world.player_data[(who / 10)].playerName == "Nick") {
            world.player_data[who / 10].weapon = "fireball";
            this->loadAnimation(who, "nick.ani");
            view.LoadTexture((who / 10) + 1, "joel.png", 254, 258);
            world.Sprite[who].texture = (who / 10) + 1;
            for (goatorg = (who + 1); goatorg <= (who + 9); ++goatorg) {
                this->loadAnimation(goatorg, "icespike.ani");
            }
        }

        if (world.player_data[(who / 10)].playerName == "Nicky") {
            world.player_data[who / 10].weapon = "bomb";
            this->loadAnimation(who, "nicky.ani");
            view.LoadTexture((who / 10) + 1, "LilNicky.png", 84, 148);
            world.Sprite[who].texture = (who / 10) + 1;
            for (goatorg = (who + 1); goatorg <= (who + 9); ++goatorg) {
                this->loadAnimation(goatorg, "bomb.ani");
            }
        }

        for (goatorg = (who + 1); goatorg <= (who + 9); ++ goatorg) {
            world.Sprite[goatorg].name = "";
            world.Sprite[goatorg].zOrder = -90;
        }
        //Sprite(who).name = what//playerName(who / 10)
        //Sprite(who).frame = 1
        //Call initSprites(who)
    }

    void debugLevel() {
        LP3_ASSERT(false); // TODO
    }

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

        sound.silence_sfx();
        //for (int j = 1; j <= 15; ++ j) {
        //    sound.PlayWave("nothing.wav");
        //}

     dogyup:

        sound.PlayIsoWave("nothing.wav");

        if (how != 2) {
            sound.PlayBgm("");
        }

        view.LoadTexture(-1, "PlainBlack.png", 25, 25);
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

    bool flipGame() {
        // I think this handles switching to different rooms or levels.
        int penguin;
        (void)penguin;  //2017- is this unused?

        if (world.exitS == "true" && boost::starts_with(world.screen, "Level")) {
            double sapple = boost::lexical_cast<double>(world.screen.substr(5));
            sapple = sapple + 0.1; // WTF, right? It's in the original code though...
            world.screen = str(boost::format("Level%s") % sapple);
			this->exec(
				new LegacyGame(this->get_process_space(), view, sound, vb, world));
			return true;
        } // End If


        if (world.screen == "SelectPlayerz") {
            this->selectPlayerS();
        }

        //playWave "conzero.wav"
        if (world.screen == "title") {
            //playWave "conTen.wav"
            world.screen = "title2";
            this->exec(
                create_title_screen(
					get_process_space(), view, sound, vb, world));
            return true;
        }

		// MOVED THIS TO CONSTRUCTOR:
        //////************************************************************
        ////// LOADS A NEW LEVEL!------------------------------------
        //////************************************************************
        ////if (boost::starts_with(world.screen, "Level")
        ////    && world.currentScreen != world.screen) {
        ////    //as long as this is set to this crazy value, it won't load it again.
        ////    world.currentScreen = world.screen;

        ////    std::string crapple = world.screen.substr(5);
        ////    double boogeycrap = boost::lexical_cast<double>(crapple);

        ////    this->goToLevel(boogeycrap);
        ////}

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

        return false;
    }

    void findPlayers() {
        world.numberPlayers = 0;
        if (world.player_data[0].playerName == "") { world.player_data[0].playerName = "zgjkl"; }
        if (world.player_data[1].playerName == "") { world.player_data[1].playerName = "zgjkl"; }
        if (world.player_data[2].playerName == "") { world.player_data[2].playerName = "zgjkl"; }


        if (world.Sprite[0].name == world.player_data[0].playerName) {
            world.numberPlayers = 1;
        }
        if (world.Sprite[10].name == world.player_data[1].playerName) {
            world.numberPlayers = 4;
        }
        if (world.Sprite[20].name == world.player_data[2].playerName) {
            world.numberPlayers = 5;
        }
        if (world.Sprite[0].name == world.player_data[0].playerName
            && world.Sprite[10].name == world.player_data[1].playerName) {
            world.numberPlayers = 2;
        }
        if (world.Sprite[0].name == world.player_data[0].playerName
            && world.Sprite[20].name == world.player_data[2].playerName) {
            world.numberPlayers = 6;
        }
        if (world.Sprite[10].name == world.player_data[1].playerName
            && world.Sprite[20].name == world.player_data[2].playerName) {
            world.numberPlayers = 7;
        }
        if (world.Sprite[0].name == world.player_data[0].playerName
            && world.Sprite[10].name == world.player_data[1].playerName
            && world.Sprite[20].name == world.player_data[2].playerName) {
            world.numberPlayers = 3;
        }
    }

    int findQ(const std::string & who) {
        int opera = 0;
        int goatX = 0;


        goatX = 0;
        for (opera = 0; opera <= world.spritesInUse; ++ opera) {
            if (world.Sprite[opera].name == who) { goatX = goatX + 1; }
        }

        return goatX;
    }

    void gameOver() {
        sound.PlayBgm("");
        world.screen = "gameOver";
        this->destroyEverything();
        view.LoadTexture(0, "GameOver.png", 320, 287);
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
        sound.PlayWave("GameOver.wav");
        this->findOrder();
    }

    double getMiddleX(int who) {
        return world.Sprite[who].x + (world.Sprite[who].wide / 2);
    }

    double getMiddleY(int who) {
        return world.Sprite[who].y + (world.Sprite[who].high / 2);
    }

    double getProx(int who, int who2) {
        // Finds who is closet
        return std::abs(getMiddleX(who) - getMiddleX(who2)) + std::abs(getMiddleY(who) - getMiddleY(who));
        // amount of pixels they are close
    }

    void GoSub_level1a() {
        world.Sprite[31].frame = world.cinema[world.cinemaCounter].frame1;
        world.Sprite[32].frame = world.cinema[world.cinemaCounter].frame2;
        world.Sprite[33].frame = world.cinema[world.cinemaCounter].frame3;
        world.Sprite[34].frame = world.cinema[world.cinemaCounter].frame4;
        world.Sprite[31].color = view.QBColor(world.cinema[world.cinemaCounter].color1);
        world.Sprite[32].color = view.QBColor(world.cinema[world.cinemaCounter].color2);
        world.Sprite[33].color = view.QBColor(world.cinema[world.cinemaCounter].color3);
        world.Sprite[34].color = view.QBColor(world.cinema[world.cinemaCounter].color4);

        sound.PlayIsoWave(world.cinema[world.cinemaCounter].wavefile);
        world.Sprite[30].miscTime = world.clock + world.cinema[world.cinemaCounter].miscTime;
        world.cinemaCounter = world.cinemaCounter + 1;
    }

    void GoSub_level1b() {
        for (int i = 31; i <= 34; ++ i) {
            if (i == 30) { continue; } //2017: WAT?
            world.Sprite[i].visible = true;
            world.Sprite[i].wide = (271 - 137) * 2;
            world.Sprite[i].high = 81 * 2;
            world.Sprite[i].seekx = 0;
            world.Sprite[i].seeky = 1;
            world.Sprite[i].texture = 9;
        }
    }

    void GoSub_level1c() {
        world.Sprite[31].x = world.CameraX - world.Sprite[31].seekx;
        world.Sprite[31].y = world.CameraY + 20;
        world.Sprite[32].x = world.CameraX + world.CameraWidth - 268 + world.Sprite[32].seekx;
        world.Sprite[32].y = world.CameraY + 20;
        world.Sprite[33].x = world.CameraX - world.Sprite[33].seekx;
        world.Sprite[33].y = world.CameraY + world.CameraHeight - 180;
        world.Sprite[34].x = world.CameraX + world.CameraWidth - 268 + world.Sprite[33].seekx;
        world.Sprite[34].y = world.CameraY + world.CameraHeight - 180;

        for (int i = 31; i <= 34; ++ i) {
            // If i = 34 Then GoTo kiddy2
            world.Sprite[i].seekx = world.Sprite[i].seekx + world.sFactor * 6;
            world.Sprite[i].seeky = world.Sprite[i].seeky + 1;
            if (world.Sprite[i].seeky == 3) {
                world.Sprite[i].seeky = 1;
            }
            world.Sprite[i].frame = world.Sprite[i].seeky;
        }

        if (world.Sprite[32].seekx > 268) {
            for (int i = 31; i <= 34; ++ i) {
                world.Sprite[i].visible = false;
            }

            world.Sprite[30].mode = "9";
        }
    }

    void GoSub_setUpLevel1() {
        for (int i = 31; i <= 34; ++ i) {
            if (i == 30) { continue; }
            world.Sprite[i].visible = true;
            world.Sprite[i].wide = (271 - 137) * 2;
            world.Sprite[i].high = 81 * 2;
            world.Sprite[i].seekx = 268;
            world.Sprite[i].seeky = 1;
            world.Sprite[i].texture = 9;
        }
    }

    void goToLevel(const double which) {
        world.Gravity = 0;

        if (which == 1.1 || which == 1) {
            this->destroyEverything();
            this->MakeLevel("Level1Opening.ogg", "Level1.cap",
// TSNOW: This is such a hack, but honestly the graphic for the apartment
//        carpet- which already looked terrible - makes the eyes bleed
//        when the texture is manually repeated to make a 64x64 bitmap.
//        It's so much worse I'd rather only use it in Emscripten.
#ifdef LP3_COMPILE_TARGET_EMSCRIPTEN
                            "Lv1bg.png",
#else
                            "Lv1bg.bmp",
#endif
                            10, 10, "Level1Cinema.png", "Level1Cinema.ani",
                            true, true);
            world.cinemaMax = 2;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1A1.wav", 4.49);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1A2.wav", 1.87);
            world.Sprite[0].x = 50;
            world.Sprite[0].y = 220;
            world.Sprite[10].x = 50;
            world.Sprite[10].y = 220;
            world.Sprite[20].x = 50;
            world.Sprite[20].y = 220;
            world.cameraStopX = 1010;
            world.cameraStopY = 905 + 480;

            sound.LoadSound(16, "goomba.wav", "dying explosion");
            sound.LoadSound(17, "GoombaOuch.wav", "Goomba Ouch");
            sound.LoadSound(18, "KerboDie.wav", "Kerbose Die");
            sound.LoadSound(19, "KerbHurt.wav", "Kerbose Ouch");
            sound.LoadSound(20, "PutuloHurt.wav", "putulohurt");
            sound.LoadSound(21, "PutuloDie.wav", "putulodie");
        }

        if (which == 1.2) {
            this->MakeLevel("Level1.ogg", "level1b.cap", "Lv1bg2.png",
                            100, 100, "Level1BCinema.png", "Level1bCinema.ani",
                            true, true);
            world.cinemaMax = 3;
            world.cinemaCounter = 0;
            this->setCinema(0, 5, 3, 7, 16, 8, 8, 15, 8, "TalkLv1C1.wav", 3.31);
            this->setCinema(1, 5, 3, 7, 16, 8, 15, 8, 8, "TalkLv1C2.wav", 2.3);
            this->setCinema(2, 5, 3, 7, 16, 15, 8, 8, 8, "TalkLv1C3.wav", 3.05);
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
            this->MakeLevel("Level1.ogg", "level1c.cap", "lv1bg3.png",
                            10, 10, "Level1BCinema.png", "Level1bCinema.ani",
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
            this->MakeLevel("Level1.ogg", "level1d.cap", "level1birdstreet.png",
                            98, 480, "Level1DCinema.png", "Level1bCinema.ani",
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

    long hitdetection(int num1, int num2, bool whatKind=false) {
        //TSNOW: I deleted a lot of commented code here as it was already pretty
        //       confusing.
        long result = 0;

        int con1 = 0;
        int con2 = 0;
        int con3 = 0;

        int left = 0;
        int right = 0;
        int up = 0;
        int down = 0;
        //Rem-checks to see if they hit from left or right
        con1 = 0; con2 = 0; con3 = 0;
        con1 = 0;
        if ((world.Sprite[num1].x + world.Sprite[num1].wide)
                >= world.Sprite[num2].x
            && world.Sprite[num1].x < world.Sprite[num2].x) {
            con1 = 1; left = 1;
        }
        if ((world.Sprite[num2].x + world.Sprite[num2].wide) >= world.Sprite[num1].x && world.Sprite[num2].x < world.Sprite[num1].x) {
            con1 = 1; right = 1;
        }


        // 2017: unreferenced label:
        // screwthis:


        //Rem- Checks to see if they ever collide from the top to bottom (Y)
        con2 = 0;

        if ((world.Sprite[num1].y + world.Sprite[num1].high)
                >= world.Sprite[num2].y
            && world.Sprite[num1].y < world.Sprite[num2].y) {
            con2 = 1; up = 1;
        }
        //Rem- Added as of 11/27/00
        if ((world.Sprite[num2].y + world.Sprite[num2].high)
                >= world.Sprite[num1].y
            && world.Sprite[num2].y < world.Sprite[num1].y) {
            con2 = 1; down = 1;
        }

        // 2017: unreferenced label:
        // screwthis2:
        if (world.Sprite[num1].kind == 5 || world.Sprite[num2].kind == 5) {
            con3 = 1;
            goto screwthis3;
        }


        //Rem- The THIRD dimension, Z!

        //if k = j) then GoTo screwthis3

        if ((world.Sprite[num1].z + (world.Sprite[num1].length * 1.5))
                >= world.Sprite[num2].z
            && world.Sprite[num1].z < world.Sprite[num2].z) {
            con3 = 1;
        }
        if (world.Sprite[num1].z == world.Sprite[num2].z) { con3 = 1; }
        //Rem- Added as of 11/27/00
        if ((world.Sprite[num2].z + world.Sprite[num2].length * 1.5)
                >= world.Sprite[num1].z
            && world.Sprite[num2].z < world.Sprite[num1].z) {
            con3 = 1;
        }

        //TSNOW: This if statement just printed a line.
        //if (con3 == 1) { Print ""; } //Rem-Beep: //Label2.Caption = "From Top of " + Str$(j)

        screwthis3:
        if (con1 == 1 && con2 == 1 && con3 == 1) { result = 1; }
        if (whatKind == true) {
            if (left == 1) { result = 2; }
            if (right == 1) { result = 3; }
            if (up == 1) { result = 4; }
            if (down == 1) { result = 5; }
            if (left == 1 && up == 1) { result = 6; }
            if (left == 1 && down == 1) { result = 7; }
            if (right == 1 && up == 1) { result = 8; }
            if (right == 1 && down == 1) { result = 9; }
        }

        return result;
    }


    void initPlayers() {
               //1 Only player 1
        //2 Player 1 and 2
        //3 All three Players
        //4 Just player 2
        //5 Just player 3
        //6 Players 1 and 3
        //7 Players 2 and 3
        int j = 0;
        world.Sprite[0].name = "dead";
        world.Sprite[10].name = "dead";
        world.Sprite[20].name = "dead";

        if (world.numberPlayers == 1) { world.gotFocus = 0; }
        if (world.numberPlayers == 2) { world.gotFocus = -2; }
        if (world.numberPlayers == 3) { world.gotFocus = -3; }
        if (world.numberPlayers == 4) { world.gotFocus = 10; }
        if (world.numberPlayers == 5) { world.gotFocus = 5; }

        //Dim j As Integer
        int k = 0;
        for (k = 0; k <= 2; ++ k) {
            if (world.player_data[k].playerName == "") { world.player_data[k].playerName = "redead"; }
        }
        //For j = 0 To (numberPlayers - 1)//
        //
        ////Sprite(j * 10).x = 50
        //Sprite(j * 10).y = 220
        //Sprite(j * 10).name = playerName(j)
        //For k = j * 10 + 1 To j * 10 + 9
        //Sprite(k).x = 60
        //Sprite(k).y = 70
        //Sprite(k).name = "fireball"
        //Next k
        //Next j

        //Rem****************88
        //After killing the players above by setting the sprites (bodies) to "dead",
        //it reincarnates them here by calling createPlayer
        //Rem*************************************8

        view.LoadTexture(0, "System.png", 336, 397);
        for (k = 0; k <= 20; k+=10) {
            //For penguin = 0 To 2
            this->createPlayer(k);
        }


        //Rem******************************************************8
        //          THIS PART LOADS UP WEAPONS
        //Rem******************************************************8

		//TODO: The next three loops are doing the same thing for different player
		//      indices. Please merge them.

        if (world.numberPlayers == 1 || world.numberPlayers == 2
            || world.numberPlayers == 3 || world.numberPlayers == 6) {
            world.Sprite[0].name = world.player_data[0].playerName;
            for (k = 0 + 1; k <= 9; ++ k) {
                world.Sprite[k].x = 60;
                world.Sprite[k].y = 70;
                if (world.player_data[0].playerName == "Thomas"
                    || world.player_data[0].playerName == "Nick") {
                    world.Sprite[k].name = "fireball";
                }
                if (world.player_data[0].playerName == "Nicky") {
                    world.Sprite[k].name = "bomb";
                }
            }
        }

        if (world.numberPlayers == 2 || world.numberPlayers == 3
            || world.numberPlayers == 4 || world.numberPlayers == 7) {
            world.Sprite[10].name = world.player_data[1].playerName;
            for (k = 11 + 1; k <= 19; ++ k) {
                world.Sprite[k].x = 60;
                world.Sprite[k].y = 70;
                if (world.player_data[1].playerName == "Thomas"
                    || world.player_data[1].playerName == "Nick") {
                    world.Sprite[k].name = "fireball";
                }
                if (world.player_data[1].playerName == "Nicky") {
                    world.Sprite[k].name = "bomb";
                }

            }
        }

        if (world.numberPlayers == 3 || world.numberPlayers == 5
            || world.numberPlayers == 6 || world.numberPlayers == 7) {
            world.Sprite[20].name = world.player_data[2].playerName;
            for (k = 21 + 1; k <= 29; ++ k) {
                world.Sprite[k].x = 60;
                world.Sprite[k].y = 70;
                world.Sprite[k].name = "fireball";
            }
        }


        //Rem- THIS PART MAKES SPRITES DIFFERENT COLORS
        if (world.Sprite[0].name == world.Sprite[10].name) {
            world.Sprite[10].color = view.QBColor(10);
        }


        //Rem- This loads sounds into holders 0 to 14, meaning each player gets 5 sounds
        //Rem- Spring.wav is then put into slot 15
        for (j = 0; j <= 2; ++ j) {

            if (world.player_data[j].playerName == "Thomas") {
                sound.LoadSound((j * 5), "fireball.wav", "fireball");
                sound.LoadSound((j * 5) + 1, "Death.wav", "DavidDeath");
                sound.LoadSound((j * 5) + 2, "hurt.wav", "DavidHurt");
            }

            if (world.player_data[j].playerName == "Nicky") {
                sound.LoadSound((j * 5), "NickyDeath.wav", "NickyDeath");
                sound.LoadSound((j * 5) + 1, "NickyHurt.wav", "NickyHurt");
                //2017: This file doesn't work, so don't play it
                //sound.LoadSound((j * 5) + 2, "SetBomb.wav ", "set bomb");
                sound.LoadSound((j * 5) + 3, "Bomb explode.wav", "bomb explode");
            }

            if (world.player_data[j].playerName == "Nick") {
                sound.LoadSound((j * 5), "nickdeath.wav", "nickdeath");
                sound.LoadSound((j * 5) + 1, "nickhurt.wav", "nickhurt");
                sound.LoadSound((j * 5) + 2, "iceshot.wav", "iceshot");
            }
        }
        sound.LoadSound(15, "Spring.wav", "spring");
    }

    void initSprites(int which) {
        // TODO: Pass the name in rather than relying on the name variable.
        auto & spr = world.Sprite[which];
        spr.proc = load_process(spr.name);
        spr.proc->initialize(view, sound, world.clock, random, spr);
    }

    void killLimit(int jex) {
        // Rem- Kills unruly sprites who are out of bounds
        auto & s = world.Sprite[jex];
        if (s.x > world.cameraStopX || s.x < -10) { killS(jex); }

        if (s.y > world.cameraStopX || s.y < -10) { killS(jex); }
    }

    void killS(int goatX) {
                auto & s = world.Sprite[goatX];
        s.visible = false;
        s.kind = 0;
        s.name = "";
        s.trueVisible = 2;
        s.flickerTime = 0;
        s.target = -1;

        {
            auto & ws = s.SpriteVerts[0];
            ws.x = world.Sprite[goatX].x;
            ws.y = world.Sprite[goatX].y + world.Sprite[goatX].high;
            ws.tu = 0;
            ws.tv = 0.5;
            ws.rhw = 1;
            ws.color = normColor;
        }
        {
            auto & ws = s.SpriteVerts[1];
            ws.x = world.Sprite[goatX].x;
            ws.y = world.Sprite[goatX].y;
            ws.tu = 0;
            ws.tv = 0;
            ws.rhw = 1;
            ws.color = normColor;
        }
        {
            auto & ws = s.SpriteVerts[2];
            ws.x = world.Sprite[goatX].x + world.Sprite[goatX].wide;
            ws.y = world.Sprite[goatX].y + world.Sprite[goatX].high;
            ws.tu = 0.5;
            ws.tv = 0.5;
            ws.rhw = 1;
            ws.color = normColor;
        }
        {
            auto & ws = s.SpriteVerts[3];
            ws.x = world.Sprite[goatX].x + world.Sprite[goatX].wide;
            ws.y = world.Sprite[goatX].y;
            ws.tu = 0.5;
            ws.tv = 0;
            ws.rhw = 1;
            ws.color = normColor;
        }
    }

    void loadAnimation(int who, const std::string & file) {
        auto & s = world.Sprite[who];
        view.load_animation_file(s.Aframe, file);
    }

    void level1() {
        //2017: Looks like this is never called
        LP3_ASSERT(false); // TODO
    }

    //TSNOW: This looks insane. It looks as if there was too much in PlayGame,
    //       so I made this function to get called for each level... which
    //       also over time would have grown to have been too much.
    void levelR(int which, int who) {
        int k = 0;

        auto & ws = world.Sprite[who]; // with sprite

        //LEVEL 1****************************************************************
        if (which == 1) {  //LEVEL 1 ****************************

            //If .name = "bullet" Then

            //End If
            if (ws.name == "greenspring") {
                if (ws.mode == "bounce") {
                    ws.frame = ws.frame + 1;
                    if (ws.frame > 5) { ws.frame = 2; }
                    if (ws.miscTime < world.clock) { ws.mode = ""; ws.frame = 1; }
                }
            }


            if (ws.name == "clouds") {
                ws.srcx = ws.srcx + (world.sFactor * 0.5);
                ws.srcx2 = ws.srcx2 + (world.sFactor * 0.5);
                ws.Aframe[1].x = ws.Aframe[1].x + 1;
                ws.Aframe[1].x2 = ws.Aframe[1].x2 + 1;
            }



            if (ws.name == "bullet") {     ////This is a strange type of bullet that in retrospect feels more like a bubble
                //if (ws.seekx <> -1) then

                this->killLimit(who);
                this->offCameraKill(who);


                //if (ws.mode = "") then


                while(!(
                    (ws.seekx > world.cameraStopX || ws.seekx < 0)
                        || (ws.seeky > world.cameraStopY || ws.seeky < 0)
                    )) {
                    if (ws.seekx > ws.x) {
                        ws.seekx = ws.seekx + ((ws.seekx - ws.x));
                    }
                    if (ws.seekx < ws.x) {
                    ws.seekx = ws.seekx - ((ws.x - ws.seekx));
                    }

                    if (ws.seeky > ws.y) {
                        ws.seeky = ws.seeky + ((ws.seeky - ws.y));
                    }
                    if (ws.seeky < ws.y) {
                        ws.seeky = ws.seeky - ((ws.y - ws.seeky));
                    }
                }



                this->seeker(who);
                ws.frame = ws.frame + 1; if (ws.frame > 1) { ws.frame = 0; }
            }


            if (ws.name == "paulrun") {
                if (ws.mode == "") { ws.mode = "right"; }

                if (ws.mode == "right") {
                    ws.reverse = false;
                    ws.x = ws.x + world.sFactor;
                    ws.seekx = ws.seekx + world.sFactor;
                    if (ws.seekx > 100) {
                        ws.mode = "left"; ws.seekx = 0; ws.dir = ""; }
                }
                if (ws.mode == "left") {
                    ws.reverse = true;
                    ws.x = ws.x - world.sFactor;
                    ws.seekx = ws.seekx + world.sFactor;
                    if (ws.seekx > 100) {
                        ws.mode = "right"; ws.seekx = 0; ws.dir = ""; }
                }

                if (ws.seekx >= 50 && ws.dir != "done") {
                    this->shoot(who, "paulbullet",
                                world.Sprite[this->checkProx(who)].x,
                                world.Sprite[checkProx(who)].y);
                    ws.dir = "done";
                }
            }

            if (ws.name == "bluestick") {
                k = random.next() * 2 + 1;
                if (k == 1) { ws.x = ws.x + world.sFactor; }
                if (k == 2) { ws.x = ws.x - world.sFactor; }
                k = random.next() * 2 + 1;
                if (k == 1) { ws.y = ws.y + world.sFactor; }
                if (k == 2) { ws.y = ws.y - world.sFactor; }
                k = random.next() * 20 + 1;
                if (k == 1) { if (ws.z == 0) {
                    ws.jumpStart = ws.z; ws.jumpTime = world.clock; } }
            }


            if (ws.name == "pigeonbomber") {
                ws.z = ws.z + world.sFactor;
                //ws.frame = ws.frame + 1
                //if (ws.frame > 2) then ws.frame = 1

                this->seeker(who);
                if (ws.x < 1) { ws.x = world.cameraStopX; }

                if (ws.miscTime < world.clock) {
                    this->shoot(who, "bluestick",
                                world.Sprite[checkProx(who)].x,
                                world.Sprite[checkProx(who)].y);
                    ws.miscTime = world.clock + 2;
                }

            }

            return;
        }
    }

    void loadLevel(const std::string & file) {
        int j = 0;
        std::array<std::string, 10> texFile;
        std::array<int, 10> texwide;
        std::array<int, 10> texhigh;
        //TSNOW: Unnecessary: string file = world.levelPath + fileArg;
        {
            auto f = vb.OpenForInput(file);
            for (j = 0; j <= 9; ++ j) {
                if (j > 3 || j < 1) {
                    f.input(texFile[j], texwide[j], texhigh[j]);
                    view.LoadTexture(j, texFile[j], texwide[j], texhigh[j]);
                }
            }
            for (j = 40; j <= 100; ++j) {
                auto & s = world.Sprite[j];
                f.input(s.name, s.x, s.y, s.z, s.srcx, s.srcy, s.srcx2,
                        s.srcy2, s.wide, s.high, s.length, s.texture, s.visible,
                        s.kind, s.zOrder);
            }
        }
        this->findOrder();
    }

    void makeJump(int which) {
        if (world.Sprite[which].z == 0) {
            world.Sprite[which].multiJump = 0;
        }
        if (world.Sprite[which].multiJump >= world.Sprite[which].maxJump) {
            return ;
        }
        world.Sprite[which].multiJump = world.Sprite[which].multiJump + 1;
        world.Sprite[which].jumpStart = world.Sprite[which].z;
        world.Sprite[which].jumpTime = world.clock;
    }

    void MakeLevel(const std::string & lvlBgMusic, const std::string & levelFile,
        const std::string & levelBgFile, const int lvlBgWidth,
        const int lvlBgHeight,
        const std::string & CinemaBitMapFile,
        const std::string & CinemaAnimationFile,
        const bool stopMusic,
        const bool loadScreen) {
        //destroyEverything

        this->destroyEverything(2);


        if (loadScreen == true) { this->NowLoading(); }

        world.CameraWidth = 640; world.CameraHeight = 480;

        if (stopMusic == true) { sound.PlayBgm(""); }


        //playBGM ""

        //destroyEverything 2
        this->loadLevel(levelFile); //"Level1b.cap"

        world.Gravity = 20;

        int j = 0;

        world.CameraX = 0; world.CameraY = 0;
        view.LoadTexture(-1, levelBgFile, lvlBgWidth, lvlBgHeight); //"lv1bg2.png", 10, 10)
                                                                    //Call loadTexture(0, "smile.png", 255, 255)

                                                                    //Call loadTexture(4, "Level1a.png", 490, 209)
                                                                    //Call loadTexture(5, "lv1bgtw.png", 308, 341)
                                                                    //Call loadTexture(6, "goomba.png", 240, 240)
        view.LoadTexture(9, CinemaBitMapFile, 400, 400); //"lvl1bg.png",400,400


        this->initPlayers();

        //Call initPlayers
        //For j = 0 To 2
        //lives(j) = 3
        //continues(j) = 3
        //Next j


        for (j = 0; j <= world.spritesInUse; ++j) {
            this->initSprites(j);
        }

        this->findOrder();
        world.Sprite[33].name = "cinema"; world.Sprite[33].zOrder = -149;
        world.Sprite[32].name = "cinema"; world.Sprite[32].zOrder = -149;
        world.Sprite[31].name = "cinema"; world.Sprite[31].zOrder = -149;
        world.Sprite[34].name = "cinema"; world.Sprite[34].zOrder = -149;
        world.Sprite[30].zOrder = -149;
        this->loadAnimation(33, CinemaAnimationFile);
        this->loadAnimation(32, CinemaAnimationFile);
        this->loadAnimation(31, CinemaAnimationFile);
        this->loadAnimation(34, CinemaAnimationFile);
        world.Sprite[30].name = "StageWhat"; world.Sprite[30].mode = "";












        if (stopMusic == true) { sound.PlayBgm(lvlBgMusic); }


        world.exitS = "";
    }

    void NowLoading() {
        view.LoadTexture(-1, "NowLoading.png", 320, 240);
        world.CameraWidth = 320;
        world.CameraHeight = 240;
        view.UpdateSprites();
        //Call DrawStuff
        world.CameraWidth = 640;
        world.CameraHeight = 480;
        view.ForceShowBackground();
    }

    void offCameraKill(int jex) {
        auto & s = world.Sprite[jex];
        if (s.x > world.CameraX + 640 || (s.x + s.wide) < world.CameraX) {
            this->killS(jex); }
        if (s.y > world.CameraY + 480 || (s.y + s.high) < world.CameraY) {
            this->killS(jex); }
    }

    void selectPlayer() {
        // the select player screen

        this->destroyEverything();
        this->NowLoading();
        view.UpdateSprites();
        view.LoadTexture(0, "PlayerS2.png", 320, 400);
        view.LoadTexture(-1, "PlayerS.png", 320, 240);
        world.CameraWidth = 320;
        world.CameraHeight = 240;

        {
            auto & s = world.Sprite[0];
            s.x = 2 * 2;
            s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (this->anyKey(0) == 1) { s.visible = true; }
            s.name = "Selecter";
            s.frame = 1;
            s.miscTime = world.clock + 2;
        }
        {
            auto & s = world.Sprite[1];
            s.x = 106 * 2;
            s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (this->anyKey(1) == 1) { s.visible = true; };
            s.name = "Selecter";
            s.frame = 2;
            s.miscTime = world.clock + 2;
        }
        {
            auto & s = world.Sprite[2];
            s.x = 212 * 2; s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (this->anyKey(2) == 1) { s.visible = true; };
            s.name = "Selecter";
            s.miscTime = world.clock + 2;
            s.frame = 3;
        }
        this->loadAnimation(0, "Selector.ani");
        this->loadAnimation(1, "Selector.ani");
        this->loadAnimation(2, "Selector.ani");

        sound.PlayBgm("Player SelectWAV.wav");
        sound.PlayWave("Select Your Characters of Justice.wav");
    }

    void selectPlayerS() {
        // after you all select players, it gets up the results

        if (world.Sprite[0].mode == "done" || world.Sprite[0].visible == false) {
            if (world.Sprite[1].mode == "done" || world.Sprite[1].visible == false) {
                if (world.Sprite[2].mode == "done" || world.Sprite[2].visible == false) {
                    for (int i = 0; i <= 2; ++i) {
                        if (world.Sprite[i].visible == false) {
                            world.Sprite[i].mode = "";
                        }
                    }
                    if (world.Sprite[0].mode == "done") {
                        world.numberPlayers = 1; }
                    if (world.Sprite[1].mode == "done") {
                        world.numberPlayers = 4; }
                    if (world.Sprite[2].mode == "done") {
                        world.numberPlayers = 5; }
                    if (world.Sprite[0].mode == "done"
                        && world.Sprite[1].mode == "done") {
                        world.numberPlayers = 2;
                    }
                    if (world.Sprite[0].mode == "done"
                        && world.Sprite[2].mode == "done") {
                        world.numberPlayers = 6;
                    }
                    if (world.Sprite[1].mode == "done"
                        && world.Sprite[2].mode == "done") {
                        world.numberPlayers = 7;
                    }
                    if (world.Sprite[0].mode == "done"
                        && world.Sprite[1].mode == "done"
                        && world.Sprite[2].mode == "done") {
                        world.numberPlayers = 3;
                    }

                    for (int penguin = 0; penguin <= 2; ++ penguin) {
                        auto & s = world.Sprite[penguin];
                        //Next penguin
                        if (s.frame == 1) {
                            world.player_data[penguin].playerName = "Thomas"; }
                        if (s.frame == 2) {
                            world.player_data[penguin].playerName = "Nick"; }
                        if (s.frame == 3) {
                            world.player_data[penguin].playerName = "Andrew"; }
                        if (s.frame == 4) {
                            world.player_data[penguin].playerName = "Phil"; }
                        if (s.frame == 5) {
                            world.player_data[penguin].playerName = "Nicky"; }
                    }
                    //1 Only player 1
                    //2 Player 1 and 2
                    //3 All three Players
                    //4 Just player 2
                    //5 Just player 3
                    //6 Players 1 and 3
                    //7 Players 2 and 3

                    world.screen = "Level1.1";
                }
            }
        }
    }

    void script() {
        if (world.Sprite[30].mode != "3" && world.Sprite[30].mode != "2") {
            // move all the faces so that they line up
            world.Sprite[31].x = world.CameraX; //  - world.Sprite[31].seekx
            world.Sprite[31].y = world.CameraY + 20;
            world.Sprite[32].x = world.CameraX + world.CameraWidth - 268; // + world.Sprite[32].seekx
            world.Sprite[32].y = world.CameraY + 20;
            world.Sprite[33].x = world.CameraX; //  - world.Sprite[33].seekx
            world.Sprite[33].y = world.CameraY + world.CameraHeight - 180;
            world.Sprite[34].x = world.CameraX + world.CameraWidth - 268; //  + world.Sprite[33].seekx
            world.Sprite[34].y = world.CameraY + world.CameraHeight - 180;
        }

        if (world.Sprite[30].mode == "6") {
            GoSub_level1c();
        }

        if (world.Sprite[30].mode == "5") {
            GoSub_level1b();
            world.Sprite[30].mode = "6";
        }
        if (world.Sprite[30].mode == "4" && world.Sprite[30].miscTime < world.clock) {
            if (world.cinemaCounter >= world.cinemaMax) {
                world.Sprite[30].mode = "5";
            } else {
                GoSub_level1a();
            }
        // world.Sprite[30].mode = "5"
        }
        if (world.Sprite[30].mode == "2") {
            if (world.cinemaCounter > world.cinemaMax) {
                world.Sprite[30].mode = "9";
                world.Sprite[31].visible = false;
                return;
            }
            GoSub_setUpLevel1();
            world.Sprite[30].mode = "3";
        }
        if (world.Sprite[30].mode == "3") {

            // Sprite(31).x = CameraX + 300
            // Sprite(31).y = CameraY + 200
            // Sprite(31).frame = 10
            // Sprite(31).wide = 275 - 188
            // Sprite(31).high = 376 - 353

            world.Sprite[31].x = world.CameraX - world.Sprite[31].seekx;
            world.Sprite[31].y = world.CameraY + 20;
            world.Sprite[32].x = world.CameraX + world.CameraWidth - 268 + world.Sprite[32].seekx;
            world.Sprite[32].y = world.CameraY + 20;
            world.Sprite[33].x = world.CameraX - world.Sprite[33].seekx;
            world.Sprite[33].y = world.CameraY + world.CameraHeight - 180;
            world.Sprite[34].x = world.CameraX + world.CameraWidth - 268 + world.Sprite[33].seekx;
            world.Sprite[34].y = world.CameraY + world.CameraHeight - 180;

            if (world.Sprite[31].seekx < 0) {
                world.Sprite[30].mode = "4";
                world.Sprite[31].seekx = 0;
                world.Sprite[32].seekx = 0;
                world.Sprite[33].seekx = 0;
                world.Sprite[34].seekx = 0;
            }

            for (int i = 31; i <= 34; ++ i) {
                if (i == 96) { continue; }
                world.Sprite[i].seekx = world.Sprite[i].seekx - world.sFactor * 6;
                world.Sprite[i].frame = world.Sprite[i].frame + 1;
                if (world.Sprite[i].frame > 2) {
                    world.Sprite[i].frame = 1;
                }
                // Sprite(i).frame = Sprite(i).seeky
            }
        }
    }

    void seeker(int who) {
        auto & s = world.Sprite[who];
        if (s.x < s.seekx) { s.x = s.x + (s.mph * world.sFactor); }
        if (s.x > s.seekx) { s.x = s.x - (s.mph * world.sFactor); }
        if (s.y < s.seeky) { s.y = s.y + (s.mph * world.sFactor); }
        if (s.y > s.seeky) { s.y = s.y - (s.mph * world.sFactor); }
    }

    void setCinema(const int who, const int frame1, const int frame2,
                   const int frame3, const int frame4, const int color1,
                   const int color2, const int color3, const int color4,
                   const std::string & wavefile, const double miscTime) {
        auto & c = world.cinema[who];
        c.frame1 = frame1;
        c.frame2 = frame2;
        c.frame3 = frame3;
        c.frame4 = frame4;
        c.color1 = color1;
        c.color2 = color2;
        c.color3 = color3;
        c.color4 = color4;
        c.wavefile = wavefile;
        c.miscTime = miscTime;
    }

    void shoot(int who, const std::string & what, int wherex, int wherey) {
        int opera;

        for (opera = (who + 1); opera <= world.spritesInUse; ++ opera) {
            if (world.Sprite[opera].name == "" || world.Sprite[opera].name == "empty" || world.Sprite[opera].name == "dead") {
                // killS opera
                world.Sprite[opera].name = what;
                break;
            }
        }

        if (opera >= 95) { return; }  //2017: WAT?

        world.Sprite[opera].trueVisible = 0;
        world.Sprite[opera].visible = true;
        world.Sprite[opera].flickOn = false;
        world.Sprite[opera].texture = world.Sprite[who].texture;
        world.Sprite[opera].wide = world.Sprite[who].wide;
        world.Sprite[opera].high = world.Sprite[who].high;

        initSprites(opera);
        world.Sprite[opera].zOrder = -1;
        world.Sprite[opera].x = world.Sprite[who].x;
        world.Sprite[opera].y = world.Sprite[who].y;
        world.Sprite[opera].z = world.Sprite[who].z;
        world.Sprite[opera].seekx = wherex;
        world.Sprite[opera].seeky = wherey;
        findOrder();
    }

};  // end of GameImpl class


gsl::owner<GameProcess *> create_legacy_screen(
	GameProcessSpace & space,
	view::View & view, Sound & sound, Vb & vb, World & world,
	std::array<boost::optional<std::string>, 3> players)
{
	//TODO: Use players, somehow
	return new LegacyGame(space, view, sound, vb, world);
}

}   }  // end namespace
