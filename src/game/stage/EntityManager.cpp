#include "EntityManager.hpp"
#include "CharacterProc.hpp"
#include "../../Sound.hpp"

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

namespace {

    void on_collision(Sound & sound, const double clock,
                      CharacterSprite & sprite_j, CharacterSprite & sprite_k)
    {
        auto gosub_hurtj = [clock, &sound, &sprite_j]() {
            sprite_j.flickerTime
                = clock + sprite_j.invTime;
            sound.PlaySound(sprite_j.soundFile);
        };

        auto gosub_hurtK = [clock, &sound, &sprite_k]() {
            sprite_k.flickerTime
                = clock + sprite_k.invTime;
            sound.PlaySound(sprite_k.soundFile);
        };

        //Player hits an enemy
        if (sprite_j.kind == Kind::player && (sprite_k.kind == Kind::enemy
            || sprite_k.kind == Kind::enemy_bullet)) {
            if (sprite_j.flickerTime < clock) {
                sprite_j.hp = sprite_j.hp - 1;
                sound.PlaySound(sprite_j.soundFile);
                gosub_hurtj();
                if (sprite_j.hp <= 0) {
                    sprite_j.proc->death_animation();
                }
            }
        }

        //Enemy hit by a fireball
        if ((sprite_j.kind == Kind::enemy || sprite_j.kind == Kind::enemy_weak_to_jumping)
            && sprite_k.kind == Kind::fireball) {

            if (sprite_j.flickerTime < clock) {
                sprite_j.hp = sprite_j.hp - 1;
                gosub_hurtj();
                if (sprite_j.hp <= 0) {
                    sprite_j.proc->death_animation();
                }
            }
            if (sprite_k.name == "fireball") {
                //if (game_state.player_data[(sprite_k.parent) / 10].slicer == false) {
                kill(sprite_k);
                // }
            }
        }

        //Player steps on a goomba type thing
        if (sprite_j.kind == Kind::player
            && (sprite_k.kind == Kind::goomba_thing || sprite_k.kind == Kind::enemy_weak_to_jumping)) {
            if (sprite_j.z > sprite_k.length
                && sprite_j.is_falling()) {

                sprite_j.start_jump(
                    (sprite_k.flickerTime < clock)
                        ? sprite_k.bounce_factor : 1.0);

                if (sprite_k.flickerTime < clock) {
                    sprite_k.hp = sprite_k.hp - 1;
                    gosub_hurtK();
                    sound.PlaySound("spring");
                    if (sprite_k.hp <= 0) {
                        sprite_k.proc->death_animation();
                    }
                }
            }
        }

        //Player illigetimately touches goomba.
        if (sprite_j.kind == Kind::player
            && (sprite_k.kind == Kind::goomba_thing || sprite_k.kind == Kind::enemy_weak_to_jumping)) {
            if (sprite_j.flickerTime < clock) {
                if (sprite_j.z < sprite_k.length
                    || !sprite_j.is_falling()) {
                    sprite_j.hp = sprite_j.hp - 1;
                    gosub_hurtj();
                    if (sprite_j.hp <= 0) {
                        sprite_j.proc->death_animation();
                    }
                }
            }
        }


        //Player jumps on bouncy object
        if (sprite_k.kind == Kind::trampoline) {

            if (sprite_j.kind == Kind::fireball || sprite_j.kind == Kind::enemy_bullet) {
                goto overalready;
            }

            //BOUNCE TIME!
            if (sprite_j.z > sprite_k.length
                && sprite_j.is_falling()) {
                sprite_j.start_jump(sprite_k.bounce_factor);
                sound.PlaySound("spring");
                sprite_k.mode = "bounce";
                sprite_k.miscTime = sprite_k.time + 1;
            }
            //OH CRAP! I NO BOUNCE
            if (sprite_j.z < sprite_k.length
                || !sprite_j.is_falling()) {
                if (sprite_j.kind == Kind::unmoveable || sprite_j.kind == Kind::fireball
                    || sprite_j.kind == Kind::trampoline) {
                    goto britneyDog2;
                }
                sprite_j.x = sprite_j.lastX;
                sprite_j.y = sprite_j.lastY;
        britneyDog2:
                EMPTY_LABEL_HACK
            }
        overalready:
            EMPTY_LABEL_HACK
        }


        if (sprite_j.kind == Kind::unmoveable) {
            if (sprite_k.kind == Kind::unmoveable || sprite_k.kind == Kind::fireball
                || sprite_k.kind == Kind::trampoline || sprite_k.kind == Kind::enemy_bullet) {
                goto britneyDog;
            }

            sprite_k.x = sprite_k.lastX;
            sprite_k.y = sprite_k.lastY;
        britneyDog:
            EMPTY_LABEL_HACK
        }
    }  // end checkHit
}


EntityManager::EntityManager(CharacterSprite & sprite)
:   s_index(0),
    sprites(gsl::make_span(&(sprite), 1))
{
}

EntityManager::EntityManager(gsl::span<CharacterSprite> sprites_arg)
:   s_index(0),
    sprites(sprites_arg)
{
}

CharacterSprite & EntityManager::grab_sprite() {
    LP3_ASSERT(s_index < sprites.size());
    auto old_index = s_index;
    s_index += 1;
    return sprites[old_index];
}

gsl::span<CharacterSprite> EntityManager::grab_sprites(int count) {
    LP3_ASSERT(count > 0);
    LP3_ASSERT(s_index + count < sprites.size());
    auto old_index = s_index;
    s_index += count;
    return gsl::make_span(&(sprites[old_index]), count);
}

void EntityManager::skip_to(int new_index) {
    LP3_ASSERT(s_index < new_index);
    s_index = new_index;
}

void EntityManager::go_back(int new_index) {
    s_index = new_index;
}

void EntityManager::run_collision_detection(Sound & sound, const double clock) {
    for (int j = 0; j < lp3::narrow<int>(sprites.size()); ++ j) {
        //If Sprite(j).mover = True Then
        for (int k = j + 1; k < lp3::narrow<int>(sprites.size()); ++k) {
            if (sprites[j].kind == Kind::neutral) { goto fthis2; }
            if (sprites[k].kind == sprites[j].kind
                || sprites[k].kind == Kind::neutral) {
                goto fthis2;
            }
            //If Sprite(k).mover = False And Sprite(j).mover = False Then GoTo fthis2
            //If k = 1 And j = 31 Then Stop
            if (touching(sprites[j], sprites[k])) {
                on_collision(sound, clock, sprites[j], sprites[k]);
                on_collision(sound, clock, sprites[k], sprites[j]);
            }
        fthis2:
            EMPTY_LABEL_HACK
        }
    }
}

EntityManagerCO::EntityManagerCO(EntityManager & manager_arg)
:   manager(manager_arg)
{}

}   }
