#ifndef NND3D_CHARACTERSPRITE_HPP
#define NND3D_CHARACTERSPRITE_HPP
#pragma once

#include <array>
#include <string>
#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include "Camera.hpp"
#include "../../AnimationFrame.hpp"
#include "../../view.hpp"

namespace nnd3d { namespace game {

class CharacterProc;

// The party a sprite belongs to; this used to be an int.
// Old docs:
//   //if kind is 0, they are neutral.  If 1, good guy.  If 2, bad guy, if 3 , good guy weapon...
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
enum class Kind {
    neutral = 0,
    player = 1,
    enemy = 2,
    fireball = 3,
    goomba_thing = 4,
    unmoveable = 5,
    trampoline = 6,
    enemy_weak_to_jumping = 7,
    enemy_bullet = 8
};

std::istream & operator >>(std::istream & in, Kind & kind);


struct SpriteLevelData {
    std::string name;
    glm::dvec3 position;
    glm::ivec2 src_ul;
    glm::ivec2 src_br;
    glm::dvec3 size;
    view::StupidIndex texture;
    bool visible;
    Kind kind;
    int z_order;
};

class CharacterSprite;

// ----------------------------------------------------------------------------
// class CharacterSpriteRef
// ----------------------------------------------------------------------------
//     Allows for other entities to hold onto a reference to a CharacterSprite.
//     The reference sprite keeps track of what points to it, and when it dies,
//     can set them all to nullptrs (obviously this is not thread safe).
//     Additionally, this is invalidated if the CharacterSprite is copied
//     and then deleted.
//     In short code using these can expect to lose the references in many
//     conditions.
// ----------------------------------------------------------------------------
class CharacterSpriteRef {
public:
    CharacterSpriteRef();
    CharacterSpriteRef(const CharacterSpriteRef & other);
    CharacterSpriteRef(CharacterSpriteRef && other);
    ~CharacterSpriteRef();

    CharacterSpriteRef & operator=(const CharacterSpriteRef & other);
    CharacterSpriteRef & operator=(CharacterSpriteRef && other);

    inline explicit operator bool() const {
        return cs != nullptr;
    }

    const CharacterSprite & operator *() const {
        LP3_ASSERT(cs != nullptr);
        return *cs;
    }

    const CharacterSprite * operator ->() const {
        LP3_ASSERT(cs != nullptr);
        return cs;
    }
    void release();

    void set(const CharacterSprite & cs);

private:

    CharacterSprite * cs;
    friend class CharacterSprite;

    void steal_reference(CharacterSpriteRef && other);
};

// ----------------------------------------------------------------------------
// class JumpRoutine
// ----------------------------------------------------------------------------
//     Handles all the logic for making a character jump. Has it's own timer
//     and must be called each frame.
// ----------------------------------------------------------------------------
class JumpRoutine {
public:
    JumpRoutine();

	// If true, this Sprite can jump on things in some contexts without
	// damage.
	inline bool is_falling() const {
		return falling;
	}

	// Sets one sprite to follow the same arc as another, by assigning most of the
	// jump vars to it. Used for Nicky's bombs originally.
	void jump_along_with(const JumpRoutine & other);

	// Start a jump. Pass in the beginning Z coordinates and, optionally, a
	// jump magnifier.
	void start(double starting_z, double jump_magnifier = 1.0);

	// Call this once per frame. The Z coordinate is passed in and returned.
    double update(const double z, const double gravity);
private:
    bool falling;
    bool jump_is_active;
    double starting_z;  // the Z coordinate when the jump started
    double jump_magnifier;  // arbitrarily magnifies the jump
    double timer;  // how long the jump has been happening
    double last_z; // the last Z coordinate, before the latest update
};

// ----------------------------------------------------------------------------
// class CharacterSprite
// ----------------------------------------------------------------------------
//     This entity drives the whole game. In the old code, all logic was
//     determined using if statements based on the settings of these; the
//     coordinates here are used for collision detection as well as drawing
//     the graphics.
//     Currently (2018-05) there is a "process" class that much of the logic
//     is being moved to. The CharacterSprite struct has a pointer to this
//     class, but hopefully this relationship will be inverted eventually.
// ----------------------------------------------------------------------------
class CharacterSprite
{
public:
     // This initializer just zeroes everything out the way it would have been
     // in Visual Basic.
     CharacterSprite();

     ~CharacterSprite();

     CharacterSprite(const CharacterSprite & other);

     CharacterSprite & operator=(const CharacterSprite & other);

     double x; //Integer
     double y; //Integer
     double lastX;
     double lastY;
     double z;  //2017: Changed to double to prevent some bugs
     double wide; //Integer 'width and height
     double high; //Integer
     double length;
     std::string dir;

     int srcx;
     int srcy;
     int srcx2;
     int srcy2;

     std::array<view::AnimationFrame, 20> Aframe;
     double seekx;
     double seeky;
     double speed;  //this is tricky.  Every time this amount of seconds pass, its little routine is updated.
     double time; // this marks the time until the sprite can do its thing again.
     int mph; //refers to how many pixels the thing can haul
     int texture;

     bool visible;
     int parent; //whose their parent sprite (what is the reason for their existence?) are they a fireball cast by sprite 0, perhaps

     int frame;
     std::string name;

     double hp; //hp!
     double mhp;

     int jump_strength;  // the intrinsic jump strength of the sprite

     JumpRoutine jump_engine;

     double flickerTime; //Lets them flicker until then
     bool flickOn;
     int trueVisible;
     double invTime; //how many seconds they flicker when hit

     std::string mode; //which mode are they in?
     Kind kind; //if kind is 0, they are neutral.  If 1, good guy.  If 2, bad guy, if 3 , good guy weapon...
     std::string deathType; //how do they die?
     double miscTime; // they must wait this long to do misc. actions
     glm::vec4 color; //lets throw some color into the mix

     std::string soundFile; //sound files

     int zOrder; //higher numbers are drawn last.
     bool drawTrue; //used in the draw ordering subprogram

     bool reverse; //to flip bitmap or not TRUE for transposing from left to right
     CharacterSpriteRef target; // it is who they are attacking

     double bounce_factor;  // per-jump value, adds to jump strength (spring uses this)

     // Behavior of this sprite.
     CharacterProc * proc;

    inline double getMiddleX() const {
        return x + (wide / 2);
    }

    inline double getMiddleY() const {
        return y + (high / 2);
    }

    inline glm::vec2 getMiddle() const {
        return glm::vec2{ getMiddleX(), getMiddleY() };
    }

    inline glm::dvec3 get_position() const {
        return { this->x, this->y, this->z };
    }

    inline glm::dvec3 get_size() const {
        return { this->wide, this->high, this->length };
    }

    inline void set_position(const glm::dvec3 & position) {
        this->x = position.x;
        this->y = position.y;
        this->z = position.z;
    }

    inline void set_seek(const glm::dvec3 & seek_location) {
        this->seekx = seek_location.x;
        this->seeky = seek_location.y;
    }

    inline void set_size(const glm::dvec3 & size) {
        this->wide = size.x;
        this->high = size.y;
        this->length = size.z;
    }

    void update_jump_physics(double gravity);

    void start_jump(double jump_magnifier=1.0);

    // Sets one sprite to follow the same arc as another, by assigning most of the
    // jump vars to it. Used for Nicky's bombs originally.
    void jump_along_with(const CharacterSprite & other);

    // If true, this Sprite can jump on things in some contexts without
    // damage.
    bool is_falling() const;

private:
    std::vector<CharacterSpriteRef *> refs;

    friend class CharacterSpriteRef;

    void invalidate_refs();
};

double getProx(const CharacterSprite & who, const CharacterSprite & who2);

// Kills the sprite if it's off camera. Returns true if killed, false otherwise.
bool off_camera_kill(CharacterSprite & sprite, const Camera & camera);

void kill(CharacterSprite & sprite);

bool touching(const CharacterSprite & a, const CharacterSprite & b);

void seek(CharacterSprite & s);

void unstretch(CharacterSprite & s);


}    }

#endif
