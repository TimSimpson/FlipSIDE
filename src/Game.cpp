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
    GameImpl(View & _view, Sound & _sound, Vb & vb,
             World & _world)
    :   vb(vb),
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

	void OnKey(const std::string & o) {
		for (int j = 0; j <= 2; ++j) {
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
		for (int j = 0; j <= 2; ++j) {
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
 //Thomas Fire
                if (world.weapon[j / 10] == "fireball"
                    && world.ThreeWay[j / 10] == false) {
                    if (world.AttackKey[penguin] == true
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
                        if (world.RightKEY[penguin] == true) {
                            world.Sprite[k].seekx
                                = s.x + (2 * world.CameraWidth);
                        }
                        if (world.LeftKEY[penguin] == true) {
                            world.Sprite[k].seekx
                                = s.x - (world.CameraWidth * 2);
                        }
                        if (world.upKey[penguin] == true) {
                            world.Sprite[k].seeky
                                = s.y - (world.CameraHeight * 2);
                        }
                        if (world.DownKEY[penguin] == true) {
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
                        world.Sprite[k].wide = 30 * (world.GradeUp[j / 10] + 1);
                        world.Sprite[k].high = 30 * (world.GradeUp[j / 10] + 1);
                        world.Sprite[k].length = 15;
                        world.Sprite[k].texture = world.Sprite[j].texture;
                        world.Sprite[k].visible = true;
                        world.Sprite[k].kind = 3;
                        //Sprite[k].soundFile = "fireball.wav"
                        world.Sprite[k].parent = j;
                        //LoadSound k, "fireball.wav"
                        if (world.playerName[world.Sprite[k].parent / 10] == "Thomas") {
                            sound.PlaySound("fireball");
                        }
                        if (world.playerName[world.Sprite[k].parent / 10] == "Nick") {
                            sound.PlaySound("iceshot");
                        }

                        s.miscTime = world.clock + 0.25;
                outofammo:
                        {
                            int cplusplusNoLikeEmptyLabelHack = 5;
                        }
                    }
                } //if thomas if

                if (world.weapon[j / 10] == "fireball"
                    && world.ThreeWay[j / 10] == true) {
                    if (world.AttackKey[penguin] == true
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
                        if (world.upKey[penguin] == true || s.dir == "u") {
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
                            if (world.LeftKEY[penguin] == true) {
                                world.Sprite[k + 2].seeky = s.y;
                                world.Sprite[k + 2].seekx
                                    = s.x - (world.CameraWidth * 2);
                                world.Sprite[k + 2].dir = "l";
                            }
                            if (world.RightKEY[penguin] == true) {
                                world.Sprite[k + 1].seeky = s.y;
                                world.Sprite[k + 1].seekx
                                    = s.x + (world.CameraWidth * 2);
                                world.Sprite[k + 1].dir = "r";
                            }
                        }
                        if (world.DownKEY[penguin] == true
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
                            if (world.LeftKEY[penguin] == true) {
                                world.Sprite[k + 2].seeky = s.y;
                                world.Sprite[k + 2].seekx
                                    = s.x - (world.CameraWidth * 2);
                                world.Sprite[k + 2].dir = "l";
                            }
                            if (world.RightKEY[penguin] == true) {
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
                                = 30 * (world.GradeUp[j / 10] + 1);
                            world.Sprite[trueorg].high
                                = 30 * (world.GradeUp[j / 10] + 1);
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
                        { const int cplusplusNoLikeEmptyLabelHack = 5; }
                    }
                } //if thomas if


                if (world.upKey[penguin] == false
                    && world.DownKEY[penguin] == false
                    && world.LeftKEY[penguin] == false
                    && world.RightKEY[penguin] == false) {
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
                    k = (int)(vb.Rnd() * 4) + 1;
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
                k = (int) (vb.Rnd() * 2) + 1;
                if (k == 1) { s.x = s.x + world.sFactor; }
                if (k == 2) { s.x = s.x - world.sFactor; }
                k = (int) (vb.Rnd() * 2) + 1;
                if (k == 1) { s.y = s.y + world.sFactor; }
                if (k == 2) { s.y = s.y - world.sFactor; }
                k = (int) (vb.Rnd() * 20) + 1;
                if (k == 1) {
                    if (s.z == 0) {
                        s.jumpStart = s.z;
                        s.jumpTime = world.clock;
                    }
                }
            }



            if (s.name == "fireball") {

                if (world.slicer[s.parent / 10] == true) {
                    if (s.color == view.QBColor(2)) {
                        s.color = view.QBColor(15);
                    }
                    if (s.color == view.QBColor(7)) { s.color = view.QBColor(2); }
                    if (s.color == view.QBColor(10)) { s.color = view.QBColor(7); }
                    if (s.color == view.QBColor(14)) {s.color = view.QBColor(10); }
                    if (s.color == view.QBColor(1)) {s.color = view.QBColor(14); }
                    if (s.color == view.QBColor(4)) { s.color = view.QBColor(1); }
                    if (s.color == view.QBColor(15)) { s.color = view.QBColor(4); }

                    if ((int) (vb.Rnd() * 1) == 1) {
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
                { int cplusplusNoLikeEmptyLabelHack = 7; }
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
                        world.lives[j / 10] = world.lives[j / 10] - 1;
                        if (world.lives[j / 10] < 1) {
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
                        if (world.lives[j / 10] > 0) {
                            this->createPlayer(j, world.playerName[j / 10]);
                            world.Sprite[j].name = world.playerName[j / 10];
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
                    world.lives[j / 10] = 2;
                    this->createPlayer(j, world.playerName[j / 10]);
                    world.Sprite[j].name = world.playerName[j / 10];
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
                    if (world.RightKEY[k] == true || world.LeftKEY[k] == true
                        || world.upKey[k] == true || world.DownKEY[k] == true
                        || world.AttackKey[k] == true) {
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

                    if (world.upKey[k] == true) {
                        s.frame = s.frame - 1;
                        if (s.frame == 0) { s.frame = 5; }
                    }
                    if (world.DownKEY[k] == true) {
                        s.frame = s.frame + 1;
                        if (s.frame == 6) { s.frame = 1; }
                    }
                    if (world.AttackKey[k] == true
                        || world.JumpKey[k] == true) {
                        s.mode = "done";
                        s.name = "KukoFax";
                        if (s.frame == 1) { sound.PlayWave("pickTom.wav"); }
                        if (s.frame == 2) { sound.PlayWave("pickNick.wav"); }
                        if (s.frame == 3) { sound.PlayWave("pickDrew.wav"); }
                        if (s.frame == 5) { sound.PlayWave("pickNicky.wav"); }
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
                            if (s.wide < 0) {
                                world.Sprite[j].mode = "?";
                                world.Sprite[30].miscTime = world.clock + 3;
                            }
                        }
                    }
                }  //mode
                if (s.mode == "?") {
                    //playWave "conten.wav"
                    {
                        auto & s2 = world.Sprite[31];
                        this->unstretch(31);
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


            if (s.name == "intro story") {
                for (k = 0; k <= 2; ++ k) {
                    if (this->anyKey(k) == 1) {
                        world.screen = "Select Player";
                    }
                }
                if (s.mode == "") {

                    world.Sprite[1].invTime = 40;
                    sound.PlayWave("introstory.wav"); // play it once then stop
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
                        view.LoadTexture(-1, "open2.bmp", 320, 240);
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
                        view.LoadTexture(-1, "open3.bmp", 320, 240);
                        {
                            auto & s2 = world.Sprite[1];
                            //.texture = 1
                            s.length = 6;
                            s.visible = true;
                            s.srcx = 2;
                            s.srcy = 189;
                            s.srcx2 = 305;
                            s.srcy2 = 254;
                            s.wide = 303 * 2;
                            s.high = (65) * 2;
                            //.x = 1: .y = 178
                        } //end with
                    }
                    if (s.mode == "word 10") {
                        view.LoadTexture(-1, "open4.bmp", 320, 240);
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
                        view.LoadTexture(-1, "open5.bmp", 320, 240);
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
                        view.LoadTexture(-1, "plainblack.bmp", 320, 240);
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
                for (penguin = 0; penguin <= 2; k+= 10) {
                    if (this->hitdetection(j, penguin) != 0
                        && world.Sprite[penguin].name
                        == world.playerName[(penguin / 10)]) {
                        world.exitS = "true";
                    }
                }
            }

            if (s.name == "tdigger") {
                if (s.mode == "") {
                    this->unstretch(j);
                    //TSNOW: Another funky step 10 loop.
                    for (penguin = 0; penguin <= 2; penguin += 10) {
                        if (this->hitdetection(j, penguin, true) == 5
                            && world.Sprite[penguin].name
                            == world.playerName[(penguin / 10)]) {
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
                    s.target = this->checkProx(j); //pickTarget(j, 1)
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

        //2017- unused label: gotDog: //end of if their ready timer thing
            { int cplusplusNoLikeEmptyLabelHack = 42; }
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
                {int cplusplusNoLikeEmptyLabelHack = 128; }
            }

        }

        this->flipGame();
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

	void TimedEvents() {
		//Rem- Sub for the laid back peacefully timed things, like animation
		//Rem- ANIMATION!
		for (int j = 0; j <= world.spritesInUse; ++j) {
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
	Vb & vb;
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

    void createPlayer(int who, const std::string & what) {
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

		sound.silence_sfx();
        //for (int j = 1; j <= 15; ++ j) {
        //    sound.PlayWave("nothing.wav");
        //}

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

        for (int j = goatorg; j <= penguin; ++ j) {
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

        for (int j = 0; j <= world.spritesInUse; ++ j) {
         world.Sprite[j].drawTrue = false;
         world.drawOrder[j] = -150;
        }

        for (int j = 0; j <= world.spritesInUse; ++ j) {
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

    void findPlayers() {
		world.numberPlayers = 0;
        if (world.playerName[0] == "") { world.playerName[0] = "zgjkl"; }
        if (world.playerName[1] == "") { world.playerName[1] = "zgjkl"; }
        if (world.playerName[2] == "") { world.playerName[2] = "zgjkl"; }


        if (world.Sprite[0].name == world.playerName[0]) {
            world.numberPlayers = 1;
        }
        if (world.Sprite[10].name == world.playerName[1]) {
            world.numberPlayers = 4;
        }
        if (world.Sprite[20].name == world.playerName[2]) {
            world.numberPlayers = 5;
        }
        if (world.Sprite[0].name == world.playerName[0]
            && world.Sprite[10].name == world.playerName[1]) {
            world.numberPlayers = 2;
        }
        if (world.Sprite[0].name == world.playerName[0]
            && world.Sprite[20].name == world.playerName[2]) {
            world.numberPlayers = 6;
        }
        if (world.Sprite[10].name == world.playerName[1]
            && world.Sprite[20].name == world.playerName[2]) {
            world.numberPlayers = 7;
        }
        if (world.Sprite[0].name == world.playerName[0]
            && world.Sprite[10].name == world.playerName[1]
            && world.Sprite[20].name == world.playerName[2]) {
            world.numberPlayers = 3;
        }
    }

    int findQ(const std::string & who) {
        LP3_ASSERT(false); // TODO
        return 0;
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
		return 0.0;
    }

    double getMiddleY(int who) {
        LP3_ASSERT(false); // TODO
		return 0.0;
    }

    double getProx(int who, int who2) {
        LP3_ASSERT(false); // TODO
		return 0.0;
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
		return 0l;
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
        auto stream = vb.OpenForInput(file);
        std::string line;
        auto & s = world.Sprite[who];
        for (int j = 1; j <= 20; ++ j) {
            if (!std::getline(stream, line)) {
                LP3_LOG_ERROR("Error reading line from %s", file);
                LP3_THROW2(lp3::core::Exception, "Failure to read line!");
            }
            if (line == "-1") {
                break;
            }
            for (int i = 0; i < line.size(); ++i) {
                if (line[i]== ',') {
                    line[i] = ' ';
                }
            }
            std::stringstream ss(line);
            ss >> s.Aframe[j].x;
            ss >> s.Aframe[j].y;
            ss >> s.Aframe[j].x2;
            ss >> s.Aframe[j].y2;
        }
        stream.close();
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
		this->loadLevel(levelFile); //"level1b.cap"

		world.Gravity = 20;

		int j = 0;

		world.CameraX = 0; world.CameraY = 0;
		view.LoadTexture(-1, levelBgFile, lvlBgWidth, lvlBgHeight); //"lv1bg2.bmp", 10, 10)
																	//Call loadTexture(0, "smile.bmp", 255, 255)

																	//Call loadTexture(4, "level1a.bmp", 490, 209)
																	//Call loadTexture(5, "lv1bgtw.bmp", 308, 341)
																	//Call loadTexture(6, "goomba.bmp", 240, 240)
		view.LoadTexture(9, CinemaBitMapFile, 400, 400); //"lvl1bg.bmp",400,400


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
        view.LoadTexture(-1, "NowLoading.bmp", 320, 240);
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
		// the select player screen

        this->destroyEverything();
        this->NowLoading();
        view.UpdateSprites();
        view.LoadTexture(0, "PlayerS2.bmp", 320, 400);
        view.LoadTexture(-1, "PlayerS.bmp", 320, 240);
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

        int penguin;
        if (world.Sprite[0].mode == "done" || world.Sprite[0].visible == false) {
            if (world.Sprite[1].mode == "done" || world.Sprite[1].visible == false) {
                if (world.Sprite[2].mode == "done" || world.Sprite[2].visible == false) {
                    for (penguin = 0; penguin <= 2; ++penguin) {
                        if (world.Sprite[penguin].visible == false) {
                            world.Sprite[penguin].mode = "";
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

                    for (penguin = 0; penguin <= 2; ++ penguin) {
                        auto & s = world.Sprite[penguin];
                        //Next penguin
                        if (s.frame == 1) {
                            world.playerName[penguin] = "Thomas"; }
                        if (s.frame == 2) {
                            world.playerName[penguin] = "Nick"; }
                        if (s.frame == 3) {
                            world.playerName[penguin] = "Andrew"; }
                        if (s.frame == 4) {
                            world.playerName[penguin] = "Phil"; }
                        if (s.frame == 5) {
                            world.playerName[penguin] = "Nicky"; }
                    }
                    //1 Only player 1
                    //2 Player 1 and 2
                    //3 All three Players
                    //4 Just player 2
                    //5 Just player 3
                    //6 Players 1 and 3
                    //7 Players 2 and 3

                    world.screen = "level1.1";
                }
            }
        }
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
		int j = 0;

		for (j = 0; j <= 2; ++j) {
			world.lives[j] = 3;
			world.continues = 2;
		}

		//playBGM ""


		//Call loadTexture(-1, "bg.bmp", 300, 300)

		this->destroyEverything();
		sound.PlayWave("OpeningWAV.wav");

		view.LoadTexture(0, "title2ALT.bmp", 285, 427);
		view.LoadTexture(1, "title1.bmp", 440, 427);
		//loadTexture 2, "goomba.bmp", 240, 240
		//Sprite(11).name = "goomba"
		//Call initSprites(11)
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

	void unstretch(int which) {
		LP3_ASSERT(false); // TODO
	}

};	// end of GameImpl class

Game::Game(View & _view, Sound & _sound, Vb & vb, World & _world)
:   impl(new GameImpl(_view, _sound, vb, _world)) {
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
