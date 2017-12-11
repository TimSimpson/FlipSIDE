#include "CinemaProc.hpp"
#include <lp3/sims.hpp>

namespace nnd3d { namespace game { namespace proc {


class Kerbose : public CharacterProc {
private:
    CharacterProcEnv env;
    CharacterSprite & sprite;
    bool dying;
	enum class Skin {
		Kerbose,
		Putulo
	} skin;
	lp3::sims::CoroutineState state;

public:
    Kerbose(CharacterProcEnv _env, EntityManager & e_manager)
    :   env(_env),
        sprite(e_manager.grab_sprite()),
        dying(false),
		state()
    {
        sprite.mover = true;
        sprite.name = "Kerbose";
        sprite.wide = 21;
        sprite.high = 19;
        sprite.visible = true;
        sprite.frame = 1;
        sprite.kind = Kind::enemy_weak_to_jumping;
        sprite.length = 10;
        sprite.deathType = "Kerbose Death";
        sprite.invTime = 1;
        sprite.hp = 3;
        sprite.soundFile = "Kerbose Ouch";
        sprite.jumpStrength = 25;
        sprite.proc = this;
        const int kerbose_skin = (int) (env.random.next() * 2.0 + 1);
		if (kerbose_skin == 1) {
			skin = Skin::Kerbose;
            env.context.view.load_animation_file(sprite.Aframe, "Kerbose.ani");
        } else {
			skin = Skin::Putulo;
            env.context.view.load_animation_file(sprite.Aframe, "Putulo.ani");
            sprite.soundFile = "putulohurt";
            sprite.deathType = "putuloDeath";
        }
    }

    void animate(std::int64_t) override {
		if (dying) {
			return;
		}
		sprite.frame = sprite.frame + 1;
        if (sprite.frame > 2) { sprite.frame = 1; }
     }

    void death_animation() override {
        sprite.visible = true;

        sprite.mover = false;
        sprite.kind = Kind::neutral;
        sprite.frame = 3;

        sprite.miscTime = env.current_time + 3;
		if (skin == Skin::Kerbose) {
			env.context.sound.PlaySound("Kerbose Die");
		} else if (skin == Skin::Putulo) {
			env.context.sound.PlaySound("putulodie");
		}
		sprite.name = "";
		dying = true;
    }

    void wiggle() {
        const auto move_x = (int)(env.random.next() * 2) + 1;
        if (move_x == 1) { sprite.x += speed_factor; }
        if (move_x == 2) { sprite.x -= speed_factor; }
        const auto move_y = (int)(env.random.next() * 2) + 1;
        if (move_y == 1) { sprite.y = sprite.y + speed_factor; }
        if (move_y == 2) { sprite.y = sprite.y - speed_factor; }
        const auto jump = (int)(env.random.next() * 20) + 1;
        if (jump == 1) {
            if (sprite.z == 0) {
                sprite.jumpStart = lp3::narrow<int>(sprite.z);
                sprite.jumpTime = env.current_time;
            }
        }
    }

    bool update() override {
		LP3_COROUTINE_BEGIN(state)
			while (!dying) {
				wiggle();
				LP3_YIELD(true);
			}

			sprite.visible = true;

			sprite.mover = false;
			sprite.kind = Kind::neutral;
			sprite.frame = 3;

			sprite.miscTime = env.current_time + 3;
			if (skin == Skin::Kerbose) {
				env.context.sound.PlaySound("Kerbose Die");
			}
			else if (skin == Skin::Putulo) {
				env.context.sound.PlaySound("putulodie");
			}

			while (sprite.miscTime >= env.current_time) {
				LP3_YIELD(true);
			}
			sprite.visible = false;
			sprite.trueVisible = 2;
			sprite.flickerTime = env.current_time + 1;
			while (sprite.miscTime + 1 >= env.current_time) {
				LP3_YIELD(true);
			}
			kill(sprite);
		LP3_COROUTINE_END()
		return false;
    }
};

CharacterProc * create_enemy_proc(
    CharacterProcEnv env,
	EntityManager & e_manager,
    const std::string & name)
{
    if (name == "Kerbose" || name == "Kirbose") {
        return new Kerbose(env, e_manager);
    }
	return nullptr;
}

}   }   }
