#include "Game.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244) 

namespace nnd3d {

namespace {
    const glm::ivec4 normColor{255, 255, 255, 255};
}


class Game::GameImpl
{
public:
    GameImpl(View & _view, Sound & _sound,
             World & _world)
    :   view(_view),
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

	void OnKey(const std::string & o) {
		for (int j = 0; j < 2; ++j) {
			if (o == world.KeyUp[j]) { world.upKey[j] = true; }
			if (o == world.KeyDown[j]) { world.DownKEY[j] = true; }
			if (o == world.KeyLeft[j]) { world.LeftKEY[j] = true; }
			if (o == world.KeyRight[j]) { world.RightKEY[j] = true; }
			if (o == world.KeyAttack[j]) { world.AttackKey[j] = true; }
			if (o == "escape") { this->endgame(); }
			if (o == "") { world.debugOn = true; }  // wtf?!
			if (o == "T") { world.exitS = "true"; sound.PlayWave("fdis.wav"); }
			if (o == "R") {
				world.slicer[0] = true;
				world.GradeUp[0] = 2;
				world.Sprite[0].wide = 25;
				world.Sprite[0].high = 25;
				sound.PlayWave("SoupedUp.wav");
			}
			if (o == "Y") { world.LemonTime = true; }
			if (o == world.KeyJump[j]) {
				world.JumpKey[j] = true;
			}
		}
	}

	void OffKey(const std::string & o) {
		for (int j = 0; j < 2; ++j) {
			if (o == world.KeyUp[j]) { world.upKey[j] = false; }
			if (o == world.KeyDown[j]) { world.DownKEY[j] = false; }
			if (o == world.KeyLeft[j]) { world.LeftKEY[j] = false; }
			if (o == world.KeyRight[j]) { world.RightKEY[j] = false; }
			if (o == world.KeyAttack[j]) { world.AttackKey[j] = false; }
			if (o == world.KeyJump[j]) { world.JumpKey[j] = false; }
		}
	}

    void PlayGame() {
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

			if (boost::starts_with(world.screen, "level")) {
				int sopoer;
				//TSNOW: This may be wrong- old code was:
				//       sopoer = Val(right(left(screen, 6), 1))
				sopoer = boost::lexical_cast<double>(world.screen.substr(6));
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

				if (world.upKey[penguin] == true) {
					if (s.dir != "u") { s.dir = "u"; s.frame = 6; }
					s.y = s.y - world.sFactor;
					//s.Frame = s.Frame + 1: if s.Frame > 6 Then s.Frame = 4
					s.speed = 0; //0.00001
					if (s.y < world.CameraY) { s.y = world.CameraY; }
				}
				if (world.DownKEY[penguin] == true) {
					if (s.dir != "d") { s.dir = "d"; s.frame = 10; }
					s.y = s.y + world.sFactor;
					//s.Frame = s.Frame + 1: if s.Frame > 9 Then s.Frame = 7
					s.speed = 0; //0.00001
					if (s.y > world.CameraY + world.CameraHeight - s.high) {
						s.y = world.CameraY + world.CameraHeight - s.high;
					}
				}
				if (world.LeftKEY[penguin] == true) {
					if (s.dir != "l" && world.upKey[penguin] == false
						&& world.DownKEY[penguin] == false) {
						s.dir = "l";
						s.frame = 14;
					}
					s.x = s.x - world.sFactor;
					//s.Frame = s.Frame + 1: if s.Frame > 12 Then s.Frame = 10
					s.speed = 0;  //0.00001
					if (s.x < world.CameraX) { s.x = world.CameraX; }
				}
				if (world.RightKEY[penguin] == true) {
					if (s.dir != "r" && world.upKey[penguin] == false
						&& world.DownKEY[penguin] == false) {
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

				if (s.name == "Nicky" && world.JumpKey[penguin] == true
					&& s.multiJump < 3) {
					world.JumpKey[penguin] = false;
					//If .z = 0 Then .multiJump = 0
					s.multiJump = s.multiJump + 1;
					s.jumpStart = s.z;
					s.jumpTime = world.clock;
				}


				if (world.JumpKey[penguin] == true && s.z == 0) {

					s.jumpStart = s.z;
					s.jumpTime = world.clock;
				}

				//Check for a lack of movement
				if (world.weapon[j / 10] == "bomb") {
					if (world.AttackKey[penguin] == true
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
						if (k == j + 10) { goto noammo; }
						world.Sprite[k].speed = 0; //0.00001
						world.Sprite[k].name = "bomb";
						world.Sprite[k].x = s.x;
						world.Sprite[k].y = s.y;
						world.Sprite[k].z = s.z; //- (Sprite(0).length);
						world.Sprite[k].wide = 30 * (world.GradeUp[j / 10] + 1);
						world.Sprite[k].high = 30 * (world.GradeUp[j / 10] + 1);
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
						sound.PlaySound("set bomb");
						//LoadSound k, "fireball.wav"
						//PlaySound "fireball"
						s.miscTime = world.clock + 0.25;

					noammo:
						int five = 5; // appease C++ label need?
					}
				} //Nicky Bomb

			} // TODO:REMOVE
		} // TODO:REMOVE
    }
	//     End Sub for playGame--------------------------------------------!!!
	//     End Sub for playGame--------------------------------------------!!!
	//     End Sub for playGame--------------------------------------------!!!
	//     End Sub for playGame--------------------------------------------!!!

	void TimedEvents() {
		//Rem- Sub for the laid back peacefully timed things, like animation
		//Rem- ANIMATION!
		for (int j = 0; j < world.spritesInUse; ++j) {
			auto & s = world.Sprite[j];

			if (s.name == "Thomas" || s.name == "Nick" && s.mode != "truck") {
				if (s.dir != "") { s.frame = s.frame + 1; }
				if (s.dir == "u") {
					if (s.frame > 8) { s.frame = 5; }
				}
				if (s.dir == "d") {
					if (s.frame > 12) { s.frame = 9; }
				}
				if (s.dir == "l") {
					if (s.frame > 16) { s.frame = 13; }
				}
				if (s.dir == "r") {
					if (s.frame > 4) { s.frame = 1; }
				}
			}

			if (s.name == "Nicky" && s.mode != "truck") {
				if (s.dir != "") { s.frame = s.frame + 1; }
				if (s.dir == "u") {
					if (s.frame > 6) { s.frame = 4; }
				}
				if (s.dir == "d") {
					if (s.frame > 9) { s.frame = 7; }
				}
				if (s.dir == "l") {
					if (s.frame > 12) { s.frame = 10; }
				}
				if (s.dir == "r") {
					if (s.frame > 3) { s.frame = 1; }
				}
			}

			if (s.name == "fireball") {
				s.frame = s.frame + 1;
				if (s.frame > 3 || s.frame < 1) { s.frame = 1; }
			}

			if (s.name == "goomba" || s.name == "Kerbose"
				|| s.name == "paulrun" || s.name == "pigeonbomber") {
				s.frame = s.frame + 1;
				if (s.frame > 2) { s.frame = 1; }
			}

			if (s.name == "pigeon") {
				s.frame = s.frame + 1;
				if (s.frame > 2) { s.frame = 1; }
			}

			if (s.name == "tdigger") {
				s.frame = s.frame + 1;
				if (s.mode == "") {
					if (s.frame > 5) { s.frame = 4; }
				}
				if (s.mode == "runner") {
					if (s.frame > 2) { s.frame = 1; }
				}
			}


			if (s.name == "bluestick") {
				s.frame = s.frame + 1;
				if (s.frame > 2) { s.frame = 1; }
			}
		}
	}

private:
    View & view;
    Sound & sound;
    World & world;

    long anyKey(int zed) {
        // Returns true if the player at the given index is pressing any key.
        return ((world.RightKEY[zed] || world.LeftKEY[zed]
                 || world.upKey[zed] || world.DownKEY[zed]
                 || world.AttackKey[zed]) ? 1 : 0);
    }

    void centerSprite(int who) {
        // Aims the camera on the given sprite.
        auto & s = world.Sprite[who];
        s.x = (world.CameraX + (world.CameraWidth / 2)) - (s.wide / 2);
        s.y = (world.CameraY + (world.CameraHeight / 2)) - (s.high / 2);
    }

	void checkHit(int j, int k) {
		LP3_ASSERT(false); // TODO
	}

    int checkProx(int who) {
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
        LP3_ASSERT(false); // TODO
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

        for (int j = 1; j < 15; ++ j) {
            sound.PlayWave("nothing.wav");
        }

     dogyup:

        sound.PlayIsoWave("nothing.wav");

        if (how != 2) {
            sound.PlayBgm("");
        }

        view.LoadTexture(-1, "PlainBlack.bmp", 25, 25);
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

    void endgame() {
        world.STOPGAME = true;
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

    long findPlayers() {
		LP3_LOG_WARNING("TODO: IMPLEMENT!");
		return 0;
    }

    void findQ(const std::string & who) {
        LP3_ASSERT(false); // TODO
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

    double getMiddleX(int who) {
        LP3_ASSERT(false); // TODO
    }

    double getMiddleY(int who) {
        LP3_ASSERT(false); // TODO
    }

    double getProx(int who, int who2) {
        LP3_ASSERT(false); // TODO
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

    long hitdetection(int num1, int num2, bool whatKind=false) {
        LP3_ASSERT(false); // TODO
    }

    void initPlayers() {
        LP3_ASSERT(false); // TODO
    }

    void initSprites(int which) {
        LP3_ASSERT(false); // TODO
    }

    void killLimit(int jex) {
        LP3_ASSERT(false); // TODO
    }

    void killS(int goatX) {
        LP3_ASSERT(false); // TODO
    }

    void loadAnimation(int who, const std::string & file) {
        LP3_ASSERT(false); // TODO
    }

    void loadframe(int jex, int whichframe,
                      int wx1, int wy1, int wx2, int wy2) {
        LP3_ASSERT(false); // TODO
    }

    void level1() {
        LP3_ASSERT(false); // TODO
    }

    void levelR(int which, int who) {
        LP3_ASSERT(false); // TODO
    }

    void loadLevel(const std::string & file) {
        LP3_ASSERT(false); // TODO
    }

    void makeJump(int which) {
        LP3_ASSERT(false); // TODO
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

	void offCameraKill(int jex) {
		LP3_ASSERT(false); // TODO
	}

	void selectPlayer() {
		LP3_ASSERT(false); // TODO
	}

	void selectPlayerS() {
		LP3_ASSERT(false); // TODO
	}

	int pickTarget(int who, int koo) {
		LP3_ASSERT(false); // TODO
	}

	void printframes(int who) {
		LP3_ASSERT(false); // TODO
	}

    void script() {
		LP3_ASSERT(false); // TODO
	}

	void seeker(int who) {
		LP3_ASSERT(false); // TODO
	}

	template<typename... Args>
	void setCinema(Args&&... args) {
		LP3_ASSERT(false); // TODO
	}

	void shoot(int who, const std::string & what, int wherex, int wherey) {
		LP3_ASSERT(false); // TODO
	}
	void titleScreen() {
		LP3_ASSERT(false); // TODO
	}

	void unstretch(int which) {
		LP3_ASSERT(false); // TODO
	}

};	// end of GameImpl class

Game::Game(View & _view, Sound & _sound,
           World & _world)
:   impl(new GameImpl(_view, _sound, _world)) {
}

Game::~Game() {
    delete impl;
}

void Game::OnKey(const std::string & c) {
    impl->OnKey(c);
}

void Game::OffKey(const std::string & c) {
    impl->OffKey(c);
}

void Game::PlayGame() {
    impl->PlayGame();
}

void Game::TimedEvents() {
    impl->TimedEvents();
}

}   // end namespace
