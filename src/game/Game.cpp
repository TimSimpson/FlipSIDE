#include "Game.hpp"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include "constants.hpp"
#include "CharacterProc.hpp"

#ifdef _MSC_VER
    // Avoid the zillions implicit conversion warnings
    #pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {


class Game::GameImpl
{
public:
    GameImpl(view::View & view_arg, Sound & sound_arg, Vb & vb_arg,
             Random & random_arg, World & world_arg)
    :   vb(vb_arg),
		view(view_arg),
        sound(sound_arg),
        world(world_arg),
        random(random_arg)
    {
        // Taken from "StartUp"
        world.debugOn = false;
        //world.FilePath = App.Path + "\"";
        //world.levelPath = App.Path + "\"";

        world.player_data[0].KeyUp = "W";
        world.player_data[0].KeyDown = "S";
        world.player_data[0].KeyLeft = "A";
        world.player_data[0].KeyRight = "D";
        world.player_data[0].KeyAttack = "F";
        world.player_data[0].KeyJump = "G";

        world.player_data[1].KeyUp = "up";
        world.player_data[1].KeyDown = "down";
        world.player_data[1].KeyLeft = "left";
        world.player_data[1].KeyRight = "right";
        world.player_data[1].KeyAttack = "O";
        world.player_data[1].KeyJump = "P";

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

        world.camera.CameraWidth = 640;  world.camera.CameraHeight = 480;
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

    void handle_input(const input::Event & event) {
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
            case input::Key::quit:
                this->endgame();
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
		if (world.numberPlayers == 0) { world.camera.gotFocus = -1; }
		if (world.numberPlayers == 1) { world.camera.gotFocus = 0; }
		if (world.numberPlayers == 2) { world.camera.gotFocus = -2; j = 0; k = 10; }
		if (world.numberPlayers == 3) { world.camera.gotFocus = -3; }
		if (world.numberPlayers == 4) { world.camera.gotFocus = 10; }
		if (world.numberPlayers == 5) { world.camera.gotFocus = 20; }
		if (world.numberPlayers == 6) { world.camera.gotFocus = -2; j = 0; k = 20; }
		if (world.numberPlayers == 7) { world.camera.gotFocus = -2; j = 10; k = 20; }
		//1 Only player 1
		//2 Player 1 and 2
		//3 All three Players
		//4 Just player 2
		//5 Just player 3
		//6 Players 1 and 3
		//7 Players 2 and 3

		//Three Player Scrolling is kind of tricky...
		if (world.camera.gotFocus == -3) {
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

		if (world.camera.gotFocus == -2) {
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

		if (world.camera.gotFocus > -1) {
			trueorg = world.Sprite[world.camera.gotFocus].x;
			penguin = world.Sprite[world.camera.gotFocus].y;
			k = world.camera.gotFocus;
		}

		//if gotFocus <> -1 Then
		//CameraX = (Sprite(gotFocus).x + (Sprite(gotFocus).wide * 0.5)) - 320
		//if CameraX < 1 Then CameraX = 1
		//if CameraX + CameraWidth >= cameraStopX Then CameraX = cameraStopX - 1 - CameraWidth
		//CameraY = (Sprite(gotFocus).y + (Sprite(gotFocus).high * 0.5)) - 240
		//if CameraY < 1 Then CameraY = 1
		//if CameraY + CameraHeight >= cameraStopY Then CameraY = cameraStopY - 1 - CameraHeight
		//End if

		if (world.camera.gotFocus != -1) {
			world.camera.CameraX = (trueorg + (world.Sprite[k].wide * 0.5)) - 320;
			if (world.camera.CameraX < 1) { world.camera.CameraX = 1; }
			if (world.camera.CameraX + world.camera.CameraWidth >= world.camera.cameraStopX) {
				world.camera.CameraX = world.camera.cameraStopX - 1 - world.camera.CameraWidth;
			}
			world.camera.CameraY = (penguin + (world.Sprite[k].high * 0.5)) - 240;
			if (world.camera.CameraY < 1) { world.camera.CameraY = 1; }
			if (world.camera.CameraY + world.camera.CameraHeight >= world.camera.cameraStopY) {
				world.camera.CameraY = world.camera.cameraStopY - 1 - world.camera.CameraHeight;
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
			if (!s.proc) {
				LP3_ASSERT(s.name == "");
            } else if (s.name == "Selecter") {
                if (current_time > s.miscTime) {
                    s.miscTime = current_time + 0.1;  //this way, the person won't scream through the selection box because it moves at 40 FPS!
                    // TSNOW: Back-door brag from 2001 ^^ 40 FPS lol. I think it actually ran at 120.
                    // TSFUTURE: Back-door brag about 2001 from 2017 ^^
                    //For k = 0 To 2
                    k = j;
                    if (s.visible == false) {
                        if (world.player_data[k].any_key_down()) {
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
            } else if (s.name == "StageWhat") {
                //
                if (s.mode == "") {
                    //playWave "conten.wav"
                    if (s.miscTime == 0) {
                        s.miscTime = current_time + 2;
                        world.Sprite[31].zOrder = -100;
                        this->findOrder();
                    }

                    {
                        //TSNOW: No idea why this is hardcoded.
                        auto & s2 = world.Sprite[31];
                        if (s.miscTime > current_time) {
                            s2.x = camera.x();
                            s2.y = camera.y();
                            s2.wide = 640;
                            s2.high = 480;
                            s2.frame = 9;
                            s2.texture = 9;
                            s2.visible = true;
                        }

                        if (s.miscTime < current_time) {
                            s2.x = s2.x + 4 * speed_factor;
                            s2.y = s2.y + 4 * speed_factor;
                            s2.wide = s2.wide - 8 * speed_factor;
                            s2.high = s2.high - 8 * speed_factor;
                            s2.zOrder = -100;
                            if (s2.wide < 0) {
                                s.mode = "?";
                                world.Sprite[30].miscTime = current_time + 3;
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
                    if (s.miscTime < current_time) {
                        s.mode = "2";
                        s.name = "script";
                        this->findOrder();
                    }
                }

            } else if (s.name == "script") {
                this->script();
            } else if (s.name == "exit") {
                //TSNOW: This looks to step up by 10 so should only iterate
                //       the loop once, but that's what the old code did.
                for (penguin = 0; penguin <= 2; penguin+= 10) {
                    if (this->hitdetection(j, penguin) != 0
                        && world.Sprite[penguin].name
                        == world.player_data[(penguin / 10)].playerName) {
                        world.exitS = "true";
                    }
                }
            } else if (s.name == "frontdoor") {
                if (this->findQ("Kerbose") < 1) {
                    this->killS(j);
                    s.name = "exit";
                    this->cinemaM(2);
                }
            } if (s.name == "tdigger") {
                if (s.mode == "") {
                    unstretch(s);
                    //TSNOW: Another funky step 10 loop.
                    for (penguin = 0; penguin <= 2; penguin += 10) {
                        if (this->hitdetection(j, penguin, true) == 5
                            && world.Sprite[penguin].name
                            == world.player_data[(penguin / 10)].playerName) {
                            s.mode = "runner";
                            s.seekx = camera.boundary().x;
                            //.mhp = 10
                            s.kind = Kind::enemy;
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

                    seek(s);

                    //Stop
                    //if (sgetMiddleX(.target) > getMiddleX(j)) then .x = .x + (sFactor * 3)
                    //if (sgetMiddleX(.target) < getMiddleX(j)) then .x = .x - (sFactor * 3)

                    //if (sgetMiddleY(.target) > getMiddleY(j)) then .y = .y + (sFactor * 1)
                    //if (sgetMiddleY(.target) < getMiddleY(j)) then .y = .y - (sFactor * 1)
                }

            } else {
				Env env(view, sound, random);
				PlayerData * player_data = nullptr;
				gsl::span<std::reference_wrapper<CharacterSprite>> children;
				// That the first 30 sprites were used for the player
				// characters. The first element was the player and the
				// next nine were considered children.
				//
				// To make it easier to transition away from this, pass
				// in i + 1 to i + 9 as a span.

				// TODO: Should `player_data` actually be the
				//       player process subclass? It would make
				//       sense to associate it.
				if (j / 10 < world.player_data.size()) {
					// Both parent sprites and children get this passed in:
					player_data = &world.player_data[j / 10];
					if (j % 10 == 0) {
						// However only parents get a list of children.

						// TODO: Maybe move all this junk somewhere else,
						// like the initialization of World
						player_data->sprite_index = j;
						player_data->sprite = &world.Sprite[j];
						player_data->player_index = j / 10;
						children = gsl::make_span(
							&world.Sprite[j + 1], &world.Sprite[j + 9]);
					}
				}
				Camera camera(world.camera);
				s.proc->update(env, world.clock, s, camera, player_data, children);
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
                if (world.Sprite[j].kind == Kind::neutral) { goto fthis2; }
                if (world.Sprite[k].kind == world.Sprite[j].kind
                    || world.Sprite[k].kind == Kind::neutral) {
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
            s.proc->animate(s);
		}
	}

private:
	Vb & vb;
    view::View & view;
    Sound & sound;
    World & world;
    Random & random;

    void centerSprite(int who) {
        // Aims the camera on the given sprite.
        auto & s = world.Sprite[who];
        s.x = (world.camera.CameraX + (world.camera.CameraWidth / 2)) - (s.wide / 2);
        s.y = (world.camera.CameraY + (world.camera.CameraHeight / 2)) - (s.high / 2);
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
        if (world.Sprite[j].kind == Kind::player && (world.Sprite[k].kind == Kind::enemy
            || world.Sprite[k].kind == Kind::enemy_bullet)) {
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
        if ((world.Sprite[j].kind == Kind::enemy || world.Sprite[j].kind == Kind::enemy_weak_to_jumping)
            && world.Sprite[k].kind == Kind::fireball) {

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
                    kill(world.Sprite[k]);
                }
            }
        }

        //Player steps on a goomba type thing
        if (world.Sprite[j].kind == Kind::player
            && (world.Sprite[k].kind == Kind::goomba_thing || world.Sprite[k].kind == Kind::enemy_weak_to_jumping)) {
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
        if (world.Sprite[j].kind == Kind::player
            && (world.Sprite[k].kind == Kind::goomba_thing || world.Sprite[k].kind == Kind::enemy_weak_to_jumping)) {
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
        if (world.Sprite[k].kind == Kind::trampoline) {

            if (world.Sprite[j].kind == Kind::fireball || world.Sprite[j].kind == Kind::enemy_bullet) {
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
                if (world.Sprite[j].kind == Kind::unmoveable || world.Sprite[j].kind == Kind::fireball
                    || world.Sprite[j].kind == Kind::trampoline) {
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


        if (world.Sprite[j].kind == Kind::unmoveable) {
            if (world.Sprite[k].kind == Kind::unmoveable || world.Sprite[k].kind == Kind::fireball
                || world.Sprite[k].kind == Kind::trampoline || world.Sprite[k].kind == Kind::enemy_bullet) {
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
        world.camera.gotFocus = -1;
        world.camera.CameraX = 0;
        world.camera.CameraY = 0;
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

		//2017: This is madness. Looks like it set `drawTrue` to false for
		//      everything, then went through and found the sprites with the
		//		highest z order first and put them in the drawOrder array.
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
        int penguin;
		(void)penguin;  //2017- is this unused?

        if (world.exitS == "true" && boost::starts_with(world.screen, "Level")) {
            double sapple = boost::lexical_cast<double>(world.screen.substr(5));
            sapple = sapple + 0.1; // WTF, right? It's in the original code though...
            world.screen = str(boost::format("Level%s") % sapple);
        } // End If

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
        if (boost::starts_with(world.screen, "Level")
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
            s.color = normal_color;
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
        world.Sprite[31].x = world.camera.CameraX - world.Sprite[31].seekx;
        world.Sprite[31].y = world.camera.CameraY + 20;
        world.Sprite[32].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[32].seekx;
        world.Sprite[32].y = world.camera.CameraY + 20;
        world.Sprite[33].x = world.camera.CameraX - world.Sprite[33].seekx;
        world.Sprite[33].y = world.camera.CameraY + world.camera.CameraHeight - 180;
        world.Sprite[34].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[33].seekx;
        world.Sprite[34].y = world.camera.CameraY + world.camera.CameraHeight - 180;

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
//		  when the texture is manually repeated to make a 64x64 bitmap.
//		  It's so much worse I'd rather only use it in Emscripten.
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
            world.camera.cameraStopX = 1010;
            world.camera.cameraStopY = 905 + 480;

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
            world.camera.cameraStopX = 1194;
            world.camera.cameraStopY = 1900;
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
            world.camera.cameraStopX = 1244;
            world.camera.cameraStopY = 2273;
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
            world.camera.cameraStopX = 3000;
            world.camera.cameraStopY = 480;
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
        if (world.Sprite[num1].kind == Kind::unmoveable || world.Sprite[num2].kind == Kind::unmoveable) {
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

        if (world.numberPlayers == 1) { world.camera.gotFocus = 0; }
        if (world.numberPlayers == 2) { world.camera.gotFocus = -2; }
        if (world.numberPlayers == 3) { world.camera.gotFocus = -3; }
        if (world.numberPlayers == 4) { world.camera.gotFocus = 10; }
        if (world.numberPlayers == 5) { world.camera.gotFocus = 5; }

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
		Env env(view, sound, random);
		spr.proc->initialize(env, world.clock, spr);
    }

    void killLimit(int jex) {
        // Rem- Kills unruly sprites who are out of bounds
        auto & s = world.Sprite[jex];
        if (s.x > world.camera.cameraStopX || s.x < -10) { kill(s); }

        if (s.y > world.camera.cameraStopX || s.y < -10) { kill(s); }
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
                off_camera_kill(world.Sprite[who]);


                //if (ws.mode = "") then


                while(!(
                    (ws.seekx > world.camera.cameraStopX || ws.seekx < 0)
                        || (ws.seeky > world.camera.cameraStopY || ws.seeky < 0)
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
                if (ws.x < 1) { ws.x = world.camera.cameraStopX; }

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
                s.proc = load_process(s.name);
            }
        }
        this->findOrder();
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

		world.camera.CameraWidth = 640; world.camera.CameraHeight = 480;

		if (stopMusic == true) { sound.PlayBgm(""); }


		//playBGM ""

		//destroyEverything 2
		this->loadLevel(levelFile); //"Level1b.cap"

		world.Gravity = 20;

		int j = 0;

		world.camera.CameraX = 0; world.camera.CameraY = 0;
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
        world.camera.CameraWidth = 320;
        world.camera.CameraHeight = 240;
        view.UpdateSprites();
        //Call DrawStuff
        world.camera.CameraWidth = 640;
        world.camera.CameraHeight = 480;
        view.ForceShowBackground();
    }

	void selectPlayer() {
		// the select player screen

        this->destroyEverything();
        this->NowLoading();
        view.UpdateSprites();
        view.LoadTexture(0, "PlayerS2.png", 320, 400);
        view.LoadTexture(-1, "PlayerS.png", 320, 240);
        world.camera.CameraWidth = 320;
        world.camera.CameraHeight = 240;

        {
            auto & s = world.Sprite[0];
            s.x = 2 * 2;
            s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (world.player_data[0].any_key_down()) { s.visible = true; }
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
            if (world.player_data[1].any_key_down()) { s.visible = true; };
            s.name = "Selecter";
            s.frame = 2;
            s.miscTime = world.clock + 2;
        }
        {
            auto & s = world.Sprite[2];
            s.x = 212 * 2; s.y = 36 * 2;
            s.wide = 105 * 2;
            s.high = (217 - 36) * 2;
            if (world.player_data[2].any_key_down()) { s.visible = true; };
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
            world.Sprite[31].x = world.camera.CameraX; //  - world.Sprite[31].seekx
            world.Sprite[31].y = world.camera.CameraY + 20;
            world.Sprite[32].x = world.camera.CameraX + world.camera.CameraWidth - 268; // + world.Sprite[32].seekx
            world.Sprite[32].y = world.camera.CameraY + 20;
            world.Sprite[33].x = world.camera.CameraX; //  - world.Sprite[33].seekx
            world.Sprite[33].y = world.camera.CameraY + world.camera.CameraHeight - 180;
            world.Sprite[34].x = world.camera.CameraX + world.camera.CameraWidth - 268; //  + world.Sprite[33].seekx
            world.Sprite[34].y = world.camera.CameraY + world.camera.CameraHeight - 180;
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

            world.Sprite[31].x = world.camera.CameraX - world.Sprite[31].seekx;
            world.Sprite[31].y = world.camera.CameraY + 20;
            world.Sprite[32].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[32].seekx;
            world.Sprite[32].y = world.camera.CameraY + 20;
            world.Sprite[33].x = world.camera.CameraX - world.Sprite[33].seekx;
            world.Sprite[33].y = world.camera.CameraY + world.camera.CameraHeight - 180;
            world.Sprite[34].x = world.camera.CameraX + world.camera.CameraWidth - 268 + world.Sprite[33].seekx;
            world.Sprite[34].y = world.camera.CameraY + world.camera.CameraHeight - 180;

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

	void titleScreen() {
		int j = 0;

		for (j = 0; j <= 2; ++j) {
			world.player_data[j].lives = 3;
			world.continues = 2;
		}

		//playBGM ""


		//Call loadTexture(-1, "bg.png", 300, 300)

		this->destroyEverything();
		sound.PlayWave("OpeningWAV.wav");

		view.LoadTexture(0, "title2ALT.png", 285, 427);
		view.LoadTexture(1, "title1.png", 440, 427);
		//loadTexture 2, "goomba.png", 240, 240
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

};	// end of GameImpl class

Game::Game(view::View & _view, Sound & _sound, Vb & vb, Random & random, World & _world)
:   impl(new GameImpl(_view, _sound, vb, random, _world)) {
}

Game::~Game() {
    delete impl;
}

void Game::handle_input(const input::Event & event) {
    impl->handle_input(event);
}

void Game::PlayGame() {
    impl->PlayGame();
}

void Game::TimedEvents() {
    impl->TimedEvents();
}

}   }  // end namespace
