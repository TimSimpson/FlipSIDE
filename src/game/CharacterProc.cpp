#include "CharacterProc.hpp"

#ifdef _MSC_VER
// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244)
#endif

namespace nnd3d { namespace game {

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
	
};

CharacterProc * load_process(const std::string &) {
    static LegacyProc legacy_proc;
    // TODO: Return other things
    return &legacy_proc;
}

}   }
