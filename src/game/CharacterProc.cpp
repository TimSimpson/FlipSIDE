#include "CharacterProc.hpp"
#include "constants.hpp"

#ifdef _MSC_VER
// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }


namespace nnd3d { namespace game {

namespace {
    void make_jump(CharacterSprite & sprite, double current_time) {
        if (sprite.z == 0) {
            sprite.multiJump = 0;
        }
        if (sprite.multiJump >= sprite.maxJump) {
            return ;
        }
        sprite.multiJump = sprite.multiJump + 1;
        sprite.jumpStart = sprite.z;
        sprite.jumpTime = current_time;
    }
}

void create_player(PlayerData & player_data, CharacterSprite & sprite,
	gsl::span<std::reference_wrapper<CharacterSprite>> & children, Env & env)
{
	LP3_ASSERT(children.size() == 9);

	if (player_data.playerName == "Thomas") {
		player_data.weapon = "fireball";
		env.view.load_animation_file(sprite.Aframe, "Thomas.ani");
		env.view.LoadTexture(player_data.sprite_index + 1, "Flip1.png", 254, 254);
		sprite.texture = player_data.sprite_index  + 1;
        for (CharacterSprite & child : children) {
		    env.view.load_animation_file(child.Aframe, "Fireball.ani");
		}
	}

	if (player_data.playerName == "Nick") {
		player_data.weapon = "fireball";
		env.view.load_animation_file(sprite.Aframe, "nick.ani");
		env.view.LoadTexture(player_data.sprite_index  + 1, "joel.png", 254, 258);
		sprite.texture = player_data.sprite_index  + 1;
        for (CharacterSprite & child : children) {
            env.view.load_animation_file(child.Aframe, "icespike.ani");
        }
	}

	if (player_data.playerName == "Nicky") {
		player_data.weapon = "bomb";
		env.view.load_animation_file(sprite.Aframe, "nicky.ani");
		env.view.LoadTexture(player_data.sprite_index  + 1, "LilNicky.png", 84, 148);
		sprite.texture = player_data.sprite_index  + 1;
        for (CharacterSprite & child : children) {
            env.view.load_animation_file(child.Aframe, "bomb.ani");
        }
	}

    for (CharacterSprite & child : children) {
		child.name = "";
		child.zOrder = -90;
	}
}


// For starters, all the old code is just shoved in here.
class LegacyProc : public CharacterProc {
public:
     LegacyProc() = default;
	 ~LegacyProc() override = default;

	 void animate(CharacterSprite & s) override {

			if ((s.name == "Thomas" || s.name == "Nick") && s.mode != "truck") {
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

    void initialize(Env & env, const double current_time, CharacterSprite & spr) override {
        //this makes all the sprites do their thing based on their name
        spr.mph = 1;
        if (spr.name == "Thomas") {
            spr.zOrder = -99;
            spr.soundFile = "DavidHurt";
            spr.wide = 40;
            spr.high = 50;
            //spr.texture = 1;
            spr.visible = true;
            spr.length = 20;
            spr.z = 0;
            spr.jumpStrength = 75;
            spr.kind = Kind::player;
            spr.frame = 5;
            spr.dir = "u";
            spr.invTime = 2;
            spr.speed = 0;
            spr.deathType = "Death of David";
            spr.hp = 4;
            //spr.mode = "truck";
            spr.mover = true;
            spr.maxJump = 1;
        }

        if (spr.name == "Nick") {
            spr.zOrder = -99;
            spr.soundFile = "nickhurt";
            spr.wide = 40;
            spr.high = 50;
            //spr.texture = 1;
            spr.visible = true;
            spr.length = 20;
            spr.z = 0;
            spr.jumpStrength = 75;
            spr.kind = Kind::player;
            spr.frame = 5;
            spr.dir = "u";
            spr.invTime = 2;
            spr.speed = 0;
            spr.deathType = "Death of Nick";
            spr.hp = 4;
            //spr.mode = "truck";
            spr.mover = true;
            spr.maxJump = 1;
        }


        if (spr.name == "Nicky") {
            spr.zOrder = -99;
            spr.soundFile = "NickyHurt";
            spr.wide = 26;
            spr.high = 30;
            //spr.texture = 1;
            spr.visible = true;
            spr.length = 20;
            spr.z = 0;
            spr.jumpStrength = 50;
            spr.kind = Kind::player;
            spr.frame = 5;
            spr.dir = "u";
            spr.invTime = 2;
            spr.speed = 0;
            spr.deathType = "Death of Nicky";
            spr.hp = 4;
            //spr.mode = "truck";
            spr.mover = true;
            spr.maxJump = 2;
        }



        if (spr.name == "Death of David") {
            //StopSound which
            //LoadSound which, "Deathspr.wav"
            env.sound.PlaySound("DavidDeath");
            spr.seekx = spr.wide * 10;
            spr.mph = 2;
            spr.kind = Kind::neutral;
            spr.frame = 17;
            env.sound.PlayWave("Death.wav");
        }
        if (spr.name == "Death of Nicky") {
            spr.srcx = 1;
            spr.srcy = 46;
            spr.srcx2 = 14;
            spr.srcy2 = 60;
            spr.name = "Death of David";
            env.sound.PlaySound("NickyDeath");
            spr.seekx = spr.wide * 10;
            spr.mph = 2;
            spr.kind = Kind::neutral;
        }
        if (spr.name == "Death of Nick") {
            spr.srcx = 1;
            spr.srcy = 46;
            spr.srcx2 = 14;
            spr.srcy2 = 60;
            spr.name = "Death of David";
            env.sound.PlaySound("nickdeath");
            spr.seekx = spr.wide * 10;
            spr.mph = 2;
            spr.kind = Kind::neutral;

        }




        if (spr.name == "tdigger") {
            env.view.load_animation_file(spr.Aframe, "tDigger.ani");
            spr.hp = 1;
        }

        if (spr.name == "clouds") {
            spr.Aframe[1].set(spr.srcx, spr.srcy, spr.srcx2, spr.srcy2);
        }


        if (spr.name == "paulrun") {
            env.view.load_animation_file(spr.Aframe, "paulrun.ani");
            spr.hp = 1;
            spr.kind = Kind::enemy_weak_to_jumping;
            spr.deathType = "gotmilkbs";
            spr.invTime = 1;
            spr.hp = 2;
            spr.soundFile = "Paul Ouch";
            spr.length = 20;
        }


        if (spr.name == "paulbullet") {
            env.view.load_animation_file(spr.Aframe, "paulbullet.ani");
            spr.hp = 999;
            spr.kind = Kind::enemy;
            spr.deathType = "Kerbose";
            spr.invTime = 0;
            spr.wide = 10;
            spr.high = 10;
            spr.name = "bullet";
            spr.mph = 2;
        }


        if (spr.name == "bs death") {
            spr.visible = true;
            spr.mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            spr.miscTime = current_time + 3;
            env.sound.PlaySound("stick die");
            spr.name = "Kerbose Death";
        }


        if (spr.name == "harharhar") {
            spr.flickerTime = current_time + 2;
            env.sound.PlayWave("harharhar.wav");
        }

        if (spr.name == "expand") {
            env.sound.PlayWave("WhaWhee.wav");
            spr.reverse = false;
        }

        if (spr.name == "cinema") {
            spr.visible = false;
            spr.frame = 8;
            unstretch(spr);
            spr.mode = "";
        }



        if (spr.name == "Dying Explosion") {
            //StopSound which
            //LoadSound which, "goombaspr.wav"
            spr.flickerTime = current_time + 10;
            spr.seekx = spr.wide * 3;
            spr.mph = 2;
            spr.kind = Kind::neutral;
            //env.sound.PlaySound(which);
            env.sound.PlaySound("dying explosion");
        }

        if (spr.name == "Kirbose" || spr.name == "Kerbose") {
            spr.mover = true;
            spr.name = "Kerbose";
            spr.wide = 21;
            spr.high = 19;
            spr.visible = true;
            spr.frame = 1;
            spr.kind = Kind::enemy_weak_to_jumping;
            spr.length = 10;
            spr.deathType = "Kerbose Death";
            spr.invTime = 1;
            spr.hp = 3;
            spr.soundFile = "Kerbose Ouch";
            spr.jumpStrength = 25;
            const int trueorg = (int) (env.random.next() * 2.0 + 1);
            if (trueorg == 1) {
                env.view.load_animation_file(spr.Aframe, "Kerbose.ani");
            } else {
                env.view.load_animation_file(spr.Aframe, "Putulo.ani");
                spr.soundFile = "putulohurt";
                spr.deathType = "putuloDeath";
            }
        }

        if (spr.name == "gotmilkbs") {
            env.sound.PlaySound("Paul Shrink");
            spr.name = "bluestick";
        }

        if (spr.name == "greenspring") {
            env.view.load_animation_file(spr.Aframe, "greenspring.ani");
            spr.frame = 1;
            spr.mode = "off";
            spr.hp = 1;
            spr.deathType = "greenspring";
            spr.kind = Kind::trampoline;
            spr.jumpM = 1.5;
            spr.length = 10;
            spr.wide = spr.wide * 2;
            spr.high = spr.high * 2;
        }


        if (spr.name == "bluestick") {
            spr.hp = 1;
            spr.kind = Kind::enemy_weak_to_jumping;
            spr.wide = 10;
            spr.high = 17;
            spr.length = 10;
            spr.Aframe[1].set(1, 173, 10, 190);
            spr.Aframe[2].set(13, 173, 23, 190);
            spr.Aframe[3].set(23, 174, 33, 190);
            spr.deathType = "bs death";
            spr.soundFile = "Stick Ouch";
            //spr.name = "Kerbose"
            env.sound.PlaySound("Stick Awaken");
            spr.frame = 1;
            unstretch(spr);
        }

        if (spr.name == "Kerbose Death") {
            spr.visible = true;
            //spr.name = "Dying Explosion"
            //spr.flickerTime = clock + 10
            //spr.seekx = spr.wide * 3
            //spr.mph = 2
            //spr.kind = 0

            spr.mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            //srcx = 114: spr.srcy = 2
            //spr.srcx2 = 134: spr.srcy2 = 19
            spr.miscTime = current_time + 3;
            env.sound.PlaySound("Kerbose Die");
        }

        if (spr.name == "putuloDeath") {
            spr.visible = true;
            spr.mover = false;
            spr.kind = Kind::neutral;
            spr.frame = 3;
            spr.miscTime = current_time + 3;
            env.sound.PlaySound("putulodie");
            spr.name = "Kerbose Death";
        }

        if (spr.name == "fireball") {
            spr.wide = 40;
            spr.high = 50;
            spr.visible = false;
            spr.texture = 1;
            spr.frame = 1;
            spr.name = "";
            spr.zOrder = -99;
        }



        if (spr.name == "Deadly Rat") {
            spr.wide = 50;
            spr.high = 50;
            spr.texture = 0;
            spr.name = "Deadly Rat";
            spr.hp = 5;
            spr.visible = true;
            spr.kind = Kind::enemy;
            spr.deathType = "Dying Explosion";
            spr.invTime = 1;
        }

        if (spr.name == "falling") {
            spr.kind = Kind::neutral;
            //spr.high = -30 //-1 * spr.high
        }

        if (spr.name == "pigeon") {
            spr.visible = true;
            spr.seekx = spr.x;
            spr.seeky = spr.y;
            spr.wide = 40;
            spr.high = 30;
            spr.hp = 1;
            spr.z = 80;
            spr.length = 30;
            spr.frame = 1;
            spr.deathType = "falling";
            spr.kind = Kind::enemy_weak_to_jumping;
        }

        if (spr.name == "pigeonbomber") {
            env.view.load_animation_file(spr.Aframe, "pigeon.ani");
            spr.visible = true;
            spr.seekx = -10;
            spr.seeky = spr.y;
            spr.wide = 40;
            spr.high = 30;
            spr.hp = 1;
            spr.z = 80;
            spr.length = 30;
            spr.frame = 1;
            spr.deathType = "falling";
            spr.kind = Kind::enemy_weak_to_jumping;
        }

        if (spr.name == "goomba") {
            spr.seekx = spr.x;
            spr.seeky = spr.y;
            spr.wide = 80;
            spr.high = 80;
            spr.length = 40;
            spr.texture = 6;
            spr.frame = 1;
            spr.mph = 1;
            spr.hp = 5;
            spr.speed = 0.25;
            spr.visible = true;
            spr.kind = Kind::goomba_thing;
            spr.invTime = 1;
            spr.deathType = "Dying Explosion";
            spr.soundFile = "Goomba Ouch";
            //LoadSound which, spr.soundFile
            spr.mover = true;
        }

        if (spr.name == "bullet") {
            spr.kind = Kind::enemy_bullet;
        }
    }

    void update(Env & env, const double current_time, CharacterSprite & s,
		        Camera & camera, PlayerData * player_data_ptr,
				gsl::span<std::reference_wrapper<CharacterSprite>> & children)
	{
                    //Rem-If s.time > clock Then GoTo gotDog
            //.time = clock + .speed
            //Rem---------------------------------------------------------------
            //'               THIS SECTION UPDATES THE DAVID SPRITE
            //Rem---------------------------------------------------------------

            if (s.name == "Thomas" || s.name == "Nicky" || s.name == "Nick") {
                int penguin = 0;
				LP3_ASSERT(player_data_ptr != nullptr);
				PlayerData & player_data = *player_data_ptr;
                if (player_data.upKey == true) {
                    if (s.dir != "u") { s.dir = "u"; s.frame = 6; }
                    s.y = s.y - speed_factor;
                    //s.Frame = s.Frame + 1: if s.Frame > 6 Then s.Frame = 4
                    s.speed = 0; //0.00001
                    if (s.y < camera.y()) { s.y = camera.y(); }
                }
                if (player_data.DownKEY == true) {
                    if (s.dir != "d") { s.dir = "d"; s.frame = 10; }
                    s.y = s.y + speed_factor;
                    //s.Frame = s.Frame + 1: if s.Frame > 9 Then s.Frame = 7
                    s.speed = 0; //0.00001
                    if (s.y > camera.y2() - s.high) {
                        s.y = camera.y2() - s.high;
                    }
                }
                if (player_data.LeftKEY == true) {
                    if (s.dir != "l" && player_data.upKey == false
                        && player_data.DownKEY == false) {
                        s.dir = "l";
                        s.frame = 14;
                    }
                    s.x = s.x - speed_factor;
                    //s.Frame = s.Frame + 1: if s.Frame > 12 Then s.Frame = 10
                    s.speed = 0;  //0.00001
                    if (s.x < camera.x()) { s.x = camera.x(); }
                }
                if (player_data.RightKEY == true) {
                    if (s.dir != "r" && player_data.upKey == false
                        && player_data.DownKEY == false) {
                        s.dir = "r";
                        s.frame = 2;
                    }
                    s.x = s.x + speed_factor;
                    //s.Frame = s.Frame + 1: if s.Frame > 3 Then s.Frame = 1
                    s.speed = 0;  //0s.00001

                    if (s.x > camera.x2() - s.wide) {
                        s.x = camera.x2() - s.wide;
                    }
                }


                if (s.z == 0) { s.multiJump = 0; }

                if (s.name == "Nicky" && player_data.JumpKey == true
                    && s.multiJump < 3) {
                    player_data.JumpKey = false;
                    //If .z = 0 Then .multiJump = 0
                    s.multiJump = s.multiJump + 1;
                    s.jumpStart = s.z;
                    s.jumpTime = current_time;
                }


                if (player_data.JumpKey == true && s.z == 0) {

                    s.jumpStart = s.z;
                    s.jumpTime = current_time;
                }

                //Check for a lack of movement
                if (player_data.weapon == "bomb") {
                    if (player_data.AttackKey == true
                        && s.miscTime < current_time) {
						for (CharacterSprite & child : children) {
                        // for (int k = j + 1; k < j + 10; ++k) {
							if (child.name == "reserved" || child.name == "")
							{
								child.speed = 0; //0.00001
								child.name = "bomb";
								child.x = s.x;
								child.y = s.y;
								child.z = s.z; //- (Sprite(0).length);
								child.wide = 30 * (player_data.GradeUp + 1);
								child.high = 30 * (player_data.GradeUp + 1);
								child.jumpStart = s.jumpStart;
								child.jumpStrength = s.jumpStrength;
								child.jumpTime = s.jumpTime;
								child.length = 15;
								child.texture = s.texture;
								child.visible = true;
								child.frame = 1;
								child.trueVisible = 1;
								child.kind = Kind::neutral;
								child.mode = "";
								child.miscTime = current_time + 3;
								child.parent = player_data.sprite_index;
								//2017: This file doesn't work:
								// sound.PlaySound("set bomb");
								//LoadSound k, "fireball.wav"
								//PlaySound "fireball"
								s.miscTime = current_time + 0.25;
							}
                        }
                    }
                } //Nicky Bomb
                //Thomas Fire
                if (player_data.weapon == "fireball"
                    && player_data.ThreeWay == false) {
                    if (player_data.AttackKey == true
                        && s.miscTime < current_time) {
						for (CharacterSprite & child : children) {
							if (child.name == "reserved" || child.name == "") {
								if (s.dir == "u") {
									child.seekx = s.x;
									child.seeky =
										s.y - (camera.height() * 2);
									child.dir = "u";
								}
								if (s.dir == "d") {
									child.seekx = s.x;
									child.seeky =
										s.y + (2 * camera.height());
									child.dir = "d";
								}
								if (s.dir == "l") {
									child.seeky = s.y;
									child.seekx
										= s.x - (camera.width() * 2);
									child.dir = "l";
								}
								if (s.dir == "r") {
									child.seeky = s.y;
									child.seekx
										= s.x + (camera.width() * 2);
									child.dir = "r";
								}
								if (player_data.RightKEY == true) {
									child.seekx
										= s.x + (2 * camera.width());
								}
								if (player_data.LeftKEY == true) {
									child.seekx
										= s.x - (camera.width() * 2);
								}
								if (player_data.upKey == true) {
									child.seeky
										= s.y - (camera.height() * 2);
								}
								if (player_data.DownKEY == true) {
									child.seeky
										= s.y + (camera.height() * 2);
								}
								if (s.mode == "truck") {
									child.seeky
										= camera.y() - camera.height();
									child.seekx = s.x;
									child.dir = "u";
								}
								//Sprite(1).visible = true
								child.speed = 0; //0.00001
								child.name = "fireball";
								child.mph = 3;
								child.x = s.x;
								child.y = s.y;
								child.z = s.z; //- (Sprite(0).length)
								child.wide = 30 * (player_data.GradeUp + 1);
								child.high = 30 * (player_data.GradeUp + 1);
								child.length = 15;
								child.texture = s.texture;
								child.visible = true;
								child.kind = Kind::fireball;
								//Sprite[k].soundFile = "fireball.wav"
								child.parent = player_data.sprite_index;
								//LoadSound k, "fireball.wav"
								if (player_data.playerName == "Thomas") {
									env.sound.PlaySound("fireball");
								}
								if (player_data.playerName == "Nick") {
									env.sound.PlaySound("iceshot");
								}

								s.miscTime = current_time + 0.25;
							}
                        }
                    }
                } //if thomas if

                if (player_data.weapon == "fireball"
                    && player_data.ThreeWay == true) {
                    if (player_data.AttackKey == true
                        && s.miscTime < current_time) {
						boost::optional<int> starting_index;
                        for (int k = 0; k <= 5; ++k) {
							CharacterSprite & child = children[k];
                            if (child.name == "reserved"
                                || child.name == "") {
								starting_index = k;
								break;
                            }
                        }
						if (!starting_index) {
                            goto outofammo3;
                        }
                        CharacterSprite & child = children[*starting_index];
                        CharacterSprite & child_2 = children[*starting_index + 1];
                        CharacterSprite & child_3 = children[*starting_index + 2];
                        if (s.dir == "l") {
                            child.seeky = s.y;
                            child.seekx
                                = s.x - (camera.width() * 2);
                            child.dir = "l";
                            child_2.seekx
                                = s.x - (camera.width() * 2);
                            child_2.seeky
                                = s.y + (camera.height() * 2);
                            child_2.dir = "l";
                            child_3.seekx
                                = s.x - (camera.width() * 2);
                            child_3.seeky
                                = s.y - (camera.height() * 2);
                            child_3.dir = "l";
                        }
                        if (s.dir == "r") {
                            child.seeky = s.y;
                            child.seekx
                                = s.x + (camera.width() * 2);
                            child.dir = "r";
                            child_2.seekx
                                = s.x + (camera.width() * 2);
                            child_2.seeky
                                = s.y + (camera.height() * 2);
                            child_2.dir = "r";
                            child_3.seekx
                                = s.x + (camera.width() * 2);
                            child_3.seeky
                                = s.y - (camera.height() * 2);
                            child_3.dir = "r";
                        }
                        if (player_data.upKey == true || s.dir == "u") {
                            child.seekx = s.x;
                            child.seeky = s.y - (camera.height() * 2);
                            child.dir = "u";
                            child_2.seekx
                                = s.x - (camera.width() * 2);
                            child_2.seeky
                                = s.y - (camera.height() * 2);
                            child_2.dir = "u";
                            child_3.seekx
                                = s.x + (camera.width() * 2);
                            child_3.seeky
                                = s.y - (camera.height() * 2);
                            child_3.dir = "u";
                            if (player_data.LeftKEY == true) {
                                child_3.seeky = s.y;
                                child_3.seekx
                                    = s.x - (camera.width() * 2);
                                child_3.dir = "l";
                            }
                            if (player_data.RightKEY == true) {
                                child_2.seeky = s.y;
                                child_2.seekx
                                    = s.x + (camera.width() * 2);
                                child_2.dir = "r";
                            }
                        }
                        if (player_data.DownKEY == true
                            || s.dir == "d") {
                            child.seekx = s.x;
                            child.seeky = s.y + (2 * camera.height());
                            child.dir = "d";
                            child_2.seekx = s.x - (camera.width() * 2);
                            child_2.seeky = s.y + (camera.height() * 2);
                            child_2.dir = "d";
                            child_3.seekx = s.x + (camera.width() * 2);
                            child_3.seeky = s.y + (camera.height() * 2);
                            child_3.dir = "d";
                            if (player_data.LeftKEY == true) {
                                child_3.seeky = s.y;
                                child_3.seekx
                                    = s.x - (camera.width() * 2);
                                child_3.dir = "l";
                            }
                            if (player_data.RightKEY == true) {
                                child_2.seeky = s.y;
                                child_2.seekx
                                    = s.x + (camera.width() * 2);
                                child_2.dir = "r";
                            }
                        }
                        if (s.mode == "truck") {
                            child.seeky
                                = camera.y() - camera.height();
                                child.seekx = s.x;
                                child.dir = "u";
                        }
                        //Sprite(1).visible = True
						for (CharacterSprite & child_itr : gsl::make_span(&children[*starting_index], &children[*starting_index] + 2)) {
                            child_itr.speed = 0;  //0.00001
                            child_itr.name = "fireball";
                            child_itr.mph = 3;
                            child_itr.x = s.x;
                            child_itr.y = s.y;
                            child_itr.z = s.z; //- (world.Sprite[0).lengh)
                            child_itr.wide
                                = 30 * (player_data.GradeUp + 1);
                            child_itr.high
                                = 30 * (player_data.GradeUp + 1);
                            child_itr.length = 15;
                            child_itr.texture
                                = s.texture;
                            child_itr.visible = true;
                            child_itr.kind = Kind::fireball;
                            child_itr.frame = 1;
                            child_itr.soundFile = "fireball.wav";
                            child_itr.parent = player_data.sprite_index;
                        }
                        //LoadSound k, "fireball.wav"
                        env.sound.PlaySound("fireball");
                        s.miscTime = current_time + 0.25;
                outofammo3:
                        EMPTY_LABEL_HACK
                    }
                } //if thomas if


                if (player_data.upKey == false
                    && player_data.DownKEY == false
                    && player_data.LeftKEY == false
                    && player_data.RightKEY == false) {
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
                seek(s);

                if (s.x == s.seekx && s.y == s.seeky) {
                    int k = (int)(env.random.next() * 4) + 1;
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
                if (s.miscTime < current_time) {
                    s.name = "";
                    s.visible = false;
                    s.trueVisible = 2;
                    s.flickerTime = current_time + 1;
                }
            }

            if (s.name == "Kerbose") {
                const int k = (int) (env.random.next() * 2) + 1;
                if (k == 1) { s.x = s.x + speed_factor; }
                if (k == 2) { s.x = s.x - speed_factor; }
                const int k2 = (int) (env.random.next() * 2) + 1;
                if (k2 == 1) { s.y = s.y + speed_factor; }
                if (k2 == 2) { s.y = s.y - speed_factor; }
                const int k3 = (int) (env.random.next() * 20) + 1;
                if (k3 == 1) {
                    if (s.z == 0) {
                        s.jumpStart = s.z;
                        s.jumpTime = current_time;
                    }
                }
            }



            if (s.name == "fireball") {

                if (player_data_ptr->slicer == true) {
                    if (s.color == env.view.QBColor(2)) {
                        s.color = env.view.QBColor(15);
                    }
                    if (s.color == env.view.QBColor(7)) { s.color = env.view.QBColor(2); }
                    if (s.color == env.view.QBColor(10)) { s.color = env.view.QBColor(7); }
                    if (s.color == env.view.QBColor(14)) {s.color = env.view.QBColor(10); }
                    if (s.color == env.view.QBColor(1)) {s.color = env.view.QBColor(14); }
                    if (s.color == env.view.QBColor(4)) { s.color = env.view.QBColor(1); }
                    if (s.color == env.view.QBColor(15)) { s.color = env.view.QBColor(4); }

                    if ((int) (env.random.next() * 1) == 1) {
                        s.jumpStrength = 75;
                        s.maxJump = 2;
                        make_jump(s, current_time);
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
                s.flickerTime = current_time + 1;
                if (s.x < s.seekx) { s.x = s.x + (s.mph * speed_factor); }
                if (s.x > s.seekx) { s.x = s.x - (s.mph * speed_factor); }
                if (s.y < s.seeky) { s.y = s.y + (s.mph * speed_factor); }
                if (s.y > s.seeky) { s.y = s.y - (s.mph * speed_factor); }
                //.wide = .wide + 1
                //.high = .high + 1
                //if (.x = .seekx And .y = .seeky Or (.x + .wide) < 1 Or .x > 640 Or (.y + .high) < 1 Or .y > 480 Then .visible = False: .name = "reserved"
                //if (.x >= .seekx And .x <= (.seekx + .mph) And .y >= .seeky And .y <= (.seeky + .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
                //if (.x <= .seekx And .x >= (.seekx - .mph) And .y <= .seeky And .y >= (.seeky - .mph) Then .visible = False: .name = "reserved": .trueVisible = 2
                if (s.x > (camera.x() + camera.width())
                    || s.x < (camera.x() - camera.width())
                    || s.y > (camera.y() + camera.height())
                    || s.y < (camera.y() - camera.height())) {
                    s.visible = false;
                    s.flickerTime = 0;
                    s.name = "reserved";
                    s.trueVisible = 2;
                }

                off_camera_kill(s, camera);
            }

            if (s.name == "bomb") {
                s.frame = s.frame + 1;
                if (s.mode != "explode") {
                    if (s.frame > 3) { s.frame = 2; }
                }
                if (s.mode == "explode") {
                    if (s.frame > 5) { s.frame = 4; }
                }
                if (s.kind == Kind::fireball && s.miscTime < current_time) {
                    s.visible = false;
                    s.flickerTime = 0;
                    s.name = "reserved";
                    s.trueVisible = 2;
                    s.kind = Kind::neutral;
                    goto fuddle;
                }
                if (s.kind == Kind::fireball) {
                    goto fuddle;
                }
                if (s.kind == Kind::neutral && s.miscTime > current_time) {
                    goto fuddle;
                }
                s.kind = Kind::fireball;
                s.miscTime = current_time + 1;
                s.mode = "explode";
                env.sound.PlaySound("bomb explode");
                s.flickerTime = current_time + 1;
            fuddle:
                EMPTY_LABEL_HACK
            }


            if (s.name == "falling") {
                //.flickerTime = clock + 1
                s.z = s.z - speed_factor;
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
                    s.x = s.x - (s.mph * 0.5 * speed_factor);
                    s.high = s.high + s.mph;
                    s.y = s.y - (s.mph * 0.5 * speed_factor);
                }
                if (s.wide >= s.seekx) {
                    s.high = s.high - s.mph;
                    s.y = s.y + (s.mph * 0.5 * speed_factor);
                    if (s.high < 1) {
                        s.name = "deceased";
                        s.visible = false;
                    }
                }
            }

            if (s.name == "Death of David") {
				LP3_ASSERT(player_data_ptr);
				PlayerData & player_data = *player_data_ptr;
				// Oh this is gross. Basically the player character changes
				// it's name at this point and uses the name to change behavior
				// but the index to know it's still the player. Even by my
				// old standards its a leaky abstraction.

				LP3_ASSERT(children.size() > 0);

                //if (.seekx = 0) then .seekx = .x: .seeky = .y
                //if (.color = QBColor(4)) then .color = QBColor(1) Else .color = QBColor(4)
                s.flickerTime = current_time + 1;
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
                        player_data.lives = player_data.lives - 1;
                        if (player_data.lives < 1) {
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

								CharacterSprite & cnt_number = children[1];
                                cnt_number.name = "continue number";
								cnt_number.color = normal_color;
								cnt_number.frame = 11;
								cnt_number.texture = 0;
								cnt_number.miscTime = current_time + 2;
								env.view.load_animation_file(
									cnt_number.Aframe, "continue.ani");

                                s.kind = Kind::neutral;
								cnt_number.kind = Kind::neutral;
								cnt_number.visible = true;
								cnt_number.wide = 20;
								cnt_number.high = 20;
                                s.y = 10;
								cnt_number.y = s.y;
                                s.y = 10;
								cnt_number.x = s.x + 100;
                            } //end continue if
                        } //end lives if
                        if (player_data.lives > 0) {
							create_player(player_data, s, children, env);
                            s.name = player_data.playerName;
                            s.kind = Kind::player;

							// TODO: Maybe make this the same somehow?
							s.proc = load_process(s.name);
							s.proc->initialize(env, current_time, s);

                            s.flickerTime = current_time + 5;
                            //Sprite[j].x = .seekx: .seekx = 0
                            //Sprite[j].y = .seekx: .seekx = 0

                        }

                    }
                }
            }

            if (s.name == "continue") {
				LP3_ASSERT(player_data_ptr);
				PlayerData & player_data = *player_data_ptr;
                if (player_data.any_key_down()) {
                    world.continues = world.continues - 1;
                    player_data.lives = 2;
					create_player(player_data, s, children, env);
                    s.name = player_data.playerName;
                    s.kind = Kind::player;
                    s.flickerTime = current_time + 5;

					s.proc = load_process(s.name);
					s.proc->initialize(env, current_time, s);
                }
                s.visible = true;
                if (player_data.player_index == 0) {
                    s.x = camera.x() + 10;
                }
                if (player_data.player_index == 1) {
                    s.x = camera.x() + 250;
                    s.color = env.view.QBColor(10);
                }
                if (player_data.player_index == 2) {
                    s.x = camera.x() + 450;
                    s.color = env.view.QBColor(14);
                }
                s.y = camera.y() + 10;
            }

            if (s.name == "continue number") {
				LP3_ASSERT(player_data_ptr);
				PlayerData & player_data = *player_data_ptr;
                s.trueVisible = 1;
                if (player_data.player_index == 0) {
                    s.x = camera.x() + 10 + 93;
                }
                if (player_data.player_index == 1) {
                    s.x = camera.x() + 250 + 93;
                }
                if (player_data.player_index == 2) {
                    s.x = camera.x() + 450 + 93;
                }
                s.y = camera.y() + 10;

                if (s.miscTime < current_time) {
                    s.miscTime = current_time + 2;
                    s.frame = s.frame - 1;
                    if (s.frame == 0) { s.frame = 13; }
                    s.visible = true;
                    if (world.continues < 1) {
                        s.frame = 12; //this is useful, say, if two people have the choice of the last continue and one gets it before someone else
                    }
                    if (s.frame == 10) { env.sound.PlayWave("ConTen.wav"); }
                    if (s.frame == 9) { env.sound.PlayWave("ConNine.wav"); }
                    if (s.frame == 8) { env.sound.PlayWave("ConEight.wav"); }
                    if (s.frame == 7) { env.sound.PlayWave("ConSeven.wav"); }
                    if (s.frame == 6) { env.sound.PlayWave("ConSix.wav"); }
                    if (s.frame == 5) { env.sound.PlayWave("ConFive.wav"); }
                    if (s.frame == 4) { env.sound.PlayWave("ConFour.wav"); }
                    if (s.frame == 3) { env.sound.PlayWave("ConThree.wav"); }
                    if (s.frame == 2) { env.sound.PlayWave("ConTwo.wav"); }
                    if (s.frame == 1) { env.sound.PlayWave("ConOne.wav"); }
                    if (s.frame == 13) { env.sound.PlayWave("ConZero.wav"); }
                    if (s.frame == 12) {
						player_data.sprite->name = "dead";
						player_data.sprite->visible = false;
						kill(s);
                    }
                }
            }

            if (s.name == "GameOverCloudTitle") {
                if (current_time > s.miscTime) {
                    s.high = s.high + 2 * speed_factor;
                    s.y = s.y - speed_factor;
                    if (s.y < 0) { s.flickerTime = current_time + 1; }
                    if (s.y < -300) {
                        s.wide = s.wide - (10 * speed_factor);
                        s.x = s.x + 5 * speed_factor;
                        if (s.wide < 0) {
                            s.visible = false;
                            world.screen = "title";
                        }
                    }
                }
            }

            if (s.name == "Title1") {
                if (s.miscTime < current_time) {
                    if (s.miscTime + 3 > current_time) {
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
                if (s.miscTime < current_time && s.mode != "part2") {
                    s.visible = true;
                    s.mode = "part2"; //: .miscTime = clock + 5
                }
            }

            if (s.name == "Title2") {
                if (s.miscTime < current_time && s.mode != "stop") {
                    s.trueVisible = 1;
                    s.flickerTime = current_time + 5;
                    s.high = s.high - 5 * speed_factor;
                    s.y = s.y + speed_factor * 2.5;
                    if (s.high < 213) {
                        s.mode = "stop";
                        s.flickerTime = current_time;
                    }
                }
            }

            if (s.name == "Title3") {
                if (s.miscTime < current_time && s.mode == "stop") {
                    world.screen = "intro story";
                }
                if (s.miscTime < current_time && s.mode != "stop") {
                    s.trueVisible = 1;
                    s.flickerTime = current_time + 5;
                    s.wide = s.wide - speed_factor * 5;
                    s.x = s.x + speed_factor * 2.5;
                    if (s.wide < 640) {
                        s.mode = "stop";
                        s.flickerTime = current_time;
                        s.miscTime = current_time + 2; //: screen = "intro story"
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
                    env.sound.PlayWave("IntroStory.ogg"); // play it once then stop
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
                        env.view.LoadTexture(-1, "Open2.png", 320, 240);
                        camera.width() = 320;
                        camera.height() = 240;
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
                        env.view.LoadTexture(-1, "Open3.png", 320, 240);
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
                        env.view.LoadTexture(-1, "Open4.png", 320, 240);
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
                        env.view.LoadTexture(-1, "Open5.png", 320, 240);
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
                        env.view.LoadTexture(-1, "PlainBlack.png", 320, 240);
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
                        s.miscTime = s.miscTime + (speed_factor * s.invTime);
                        //0s.3 * sFactor
                        s.color = env.view.Rgb(s.miscTime, s.miscTime, s.miscTime);
                    }
                    if (s.miscTime > 245) {
                        //playWave "conten.wav"
                        s.color = env.view.QBColor(15);
                        s.mode = "words1b";
                        s.miscTime = current_time + s.length;
                    }
                }

                if (world.Sprite[0].mode == "KILLDEATH" && s.mode == "words1b") {
                    s.name = "";
                }

                if (s.mode == "words1b" && s.miscTime < current_time) {
                    s.mode = "words1c";
                    s.miscTime = 255;
                }

                if (s.mode == "words1c") {
                    if (s.miscTime > 0) {
                        s.miscTime = s.miscTime - (speed_factor * 20); //0.3 * sFactor
                        if (s.miscTime > 0) {
                            s.color = env.view.Rgb(s.miscTime, s.miscTime, s.miscTime);
                        }
                    }
                    if (s.miscTime < 1) {
                        //playWave "conten.wav"
                        s.color = env.view.QBColor(0);
                        s.mode = "words2";
                    }
                }

            }


            //playWave "conten.wav"

            

            


            if (s.name == "expand") {
                s.kind = Kind::neutral;
                //if (s.mode = "runner") then
                s.frame = 3;
                s.SpriteVerts[0].rhw
                    = s.SpriteVerts[0].rhw + (0.01 * speed_factor);
                s.SpriteVerts[3].rhw
                    = s.SpriteVerts[3].rhw + (0.01 * speed_factor);
                s.wide = s.wide + (speed_factor);
                s.x = s.x - (speed_factor / 2);
                s.high = s.high + (speed_factor);
                s.y = s.y - (speed_factor / 2);
                if (s.SpriteVerts[3].rhw > 2) {
                    s.name = "harharhar";
					s.proc = load_process(s.name);
					s.proc->initialize(env, current_time, s);
                }
            }

            if (s.name == "harharhar") {
                if (s.flickerTime < current_time) {
					kill(s);
                }
            }

            if (s.name == "dead") {
                //Stop
                if ((j / 10) == 0) {
                    s.x = camera.x() + 10;
                }
                if ((j / 10) == 1) {
                    s.x = camera.x() + 250;
                    s.color = env.view.QBColor(10);
                }
                if ((j / 10) == 2) {
                    s.x = camera.x() + 450;
                    s.color = env.view.QBColor(14);
                }
                s.y = camera.y() + 10;
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
};


CharacterProc * load_process(const std::string &) {
    static LegacyProc legacy_proc;
    // TODO: Return other things
    return &legacy_proc;
}

}   }
