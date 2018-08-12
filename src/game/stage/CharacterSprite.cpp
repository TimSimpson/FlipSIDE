#include "CharacterSprite.hpp"
#include "../Game.hpp"

namespace nnd3d { namespace game {

std::istream & operator >>(std::istream & in, Kind & kind) {
    int original_int;
    in >> original_int;
    kind = static_cast<Kind>(original_int);
    return in;
}

CharacterSpriteRef::CharacterSpriteRef()
:   cs(nullptr)
{}

CharacterSpriteRef::CharacterSpriteRef(const CharacterSpriteRef & other)
:   cs(nullptr)
{
    if (other.cs) {
        // Set this to point to the same thing. This will add another entry
        // in the CharacterSprite's table.
        this->set(*other.cs);
    }
}

CharacterSpriteRef::CharacterSpriteRef(CharacterSpriteRef && other)
:   cs(nullptr)
{
    steal_reference(std::move(other));
}

CharacterSpriteRef::~CharacterSpriteRef() {
    release();
}

CharacterSpriteRef & CharacterSpriteRef::operator=(
    const CharacterSpriteRef & other)
{
    if (other.cs) {
        // Set this to point to the same thing. This will add another entry
        // in the CharacterSprite's table.
        this->set(*other.cs);
    } else {
        release();
    }
    return *this;
}

CharacterSpriteRef &  CharacterSpriteRef::operator=(CharacterSpriteRef && other)
{
    steal_reference(std::move(other));
    return *this;
}


void CharacterSpriteRef::release() {
    if (cs) {
		cs->refs.erase(
			std::remove_if(cs->refs.begin(), cs->refs.end(),
						   [this](const CharacterSpriteRef * r){
				return r == this;
			}),
			cs->refs.end()
		);
    }
    cs = nullptr;
}

void CharacterSpriteRef::set(const CharacterSprite & new_cs) {
    if (&new_cs == cs) {
        return;
    }
    release();
    cs = const_cast<CharacterSprite *>(&new_cs);
    cs->refs.push_back(this);
}

void CharacterSpriteRef::steal_reference(CharacterSpriteRef && other) {
    release();  // even if we're pointing at the same one, we want to steal
                // the other one's table entry.
    if (other.cs) {
        // Find the incoming ref in the CharacterSprite's table, and change it
        // to this location.
        for (auto & ptr: other.cs->refs) {
            if (ptr == &other) {
                ptr = this;
            }
        }
        this->cs = other.cs;
        other.cs = nullptr;
    }
}

CharacterSprite::CharacterSprite()
:   x(0),
    y(0),
    lastX(0),
    lastY(0),
    z(0),
    wide(0),
    high(0),
    length(0),
    dir(""),
    srcx(0),
    srcy(0),
    srcx2(0),
    srcy2(0),
    Aframe(),
    seekx(0),
    seeky(0),
    speed(0),
    time(0),
    mph(0),
    texture(0),
    visible(false),
    parent(0),
    frame(0),
    name(""),
    hp(0),
    mhp(0),
    // jump_is_active(false),
    // jumpStart(0),
    jump_strength(0),
    // jumpTime(0),
    // lastJump(0),
    jump_engine(),
    flickerTime(0),
    flickOn(0),
    trueVisible(0),
    invTime(0),
    mode(""),
    kind(Kind::neutral),
    deathType(""),
    miscTime(0),
    color({1.0f, 1.0f, 1.0f, 1.0f}),
    soundFile(""),
    zOrder(0),
    drawTrue(false),
    reverse(false),
    target(),
    bounce_factor(0),
    proc(),
    refs()
{
}

CharacterSprite::~CharacterSprite() {
    invalidate_refs();
}

CharacterSprite::CharacterSprite(const CharacterSprite & other)
:   x(other.x),
    y(other.y),
    lastX(other.lastX),
    lastY(other.lastY),
    z(other.z),
    wide(other.wide),
    high(other.high),
    length(other.length),
    dir(other.dir),
    srcx(other.srcx),
    srcy(other.srcy),
    srcx2(other.srcx2),
    srcy2(other.srcy2),
    Aframe(other.Aframe),
    seekx(other.seekx),
    seeky(other.seeky),
    speed(other.speed),
    time(other.time),
    mph(other.mph),
    texture(other.texture),
    visible(other.visible),
    parent(other.parent),
    frame(other.frame),
    name(other.name),
    hp(other.hp),
    mhp(other.mhp),
    // jump_is_active(false),
    // jumpStart(other.jumpStart),
    jump_strength(other.jump_strength),
    // jumpTime(other.jumpTime),
    // lastJump(other.lastJump),
    jump_engine(),
    flickerTime(other.flickerTime),
    flickOn(other.flickOn),
    trueVisible(other.trueVisible),
    invTime(other.invTime),
    mode(other.mode),
    kind(other.kind),
    deathType(other.deathType),
    miscTime(other.miscTime),
    color(other.color),
    soundFile(other.soundFile),
    zOrder(other.zOrder),
    drawTrue(other.drawTrue),
    reverse(other.reverse),
    target(),  // make this brand new - do not copy!
    bounce_factor(other.bounce_factor),
    proc(other.proc),
    refs()  // make this brand new - do not copy!
{
}

CharacterSprite & CharacterSprite::operator=(const CharacterSprite & other) {
    this->x = other.x;
    this->y = other.y;
    this->lastX = other.lastX;
    this->lastY = other.lastY;
    this->z = other.z;
    this->wide = other.wide;
    this->high = other.high;
    this->length = other.length;
    this->dir = other.dir;
    this->srcx = other.srcx;
    this->srcy = other.srcy;
    this->srcx2 = other.srcx2;
    this->srcy2 = other.srcy2;
    this->Aframe = other.Aframe;
    this->seekx = other.seekx;
    this->seeky = other.seeky;
    this->speed = other.speed;
    this->time = other.time;
    this->mph = other.mph;
    this->texture = other.texture;
    this->visible = other.visible;
    this->parent = other.parent;
    this->frame = other.frame;
    this->name = other.name;
    this->hp = other.hp;
    this->mhp = other.mhp;
    // this->jump_is_active = other.jump_is_active;
    // this->jumpStart = other.jumpStart;
    this->jump_strength = other.jump_strength;
    // this->jumpTime = other.jumpTime;
    // this->lastJump = other.lastJump;
    this->jump_engine = other.jump_engine;
    this->flickerTime = other.flickerTime;
    this->flickOn = other.flickOn;
    this->trueVisible = other.trueVisible;
    this->invTime = other.invTime;
    this->mode = other.mode;
    this->kind = other.kind;
    this->deathType = other.deathType;
    this->miscTime = other.miscTime;
    this->color = other.color;
    this->soundFile = other.soundFile;
    this->zOrder = other.zOrder;
    this->drawTrue = other.drawTrue;
    this->reverse = other.reverse;
    this->bounce_factor = other.bounce_factor;
    this->proc = other.proc;

    this->target.release();
    this->refs.clear();

    return *this;
}

void CharacterSprite::invalidate_refs() {
    for (CharacterSpriteRef * r : refs) {
        LP3_ASSERT(r != nullptr);
        r->cs = nullptr;
    }
}


double getProx(const CharacterSprite & who, const CharacterSprite & who2) {
    // Finds who is closet
    return std::abs(who.getMiddleX() - who2.getMiddleX())
        + std::abs(who.getMiddleY() - who.getMiddleY());
    // amount of pixels they are close
}


bool touching(const CharacterSprite & num1, const CharacterSprite & num2) {
    const bool con_x = (
        ((num1.x + num1.wide) >= num2.x && num1.x < num2.x)
        || ((num2.x + num2.wide) >= num1.x && num2.x < num1.x)
    );

    const bool con_y = (
        ((num1.y + num1.high) >= num2.y && num1.y < num2.y)
        || ((num2.y + num2.high) >= num1.y && num2.y < num1.y)
    );

    const bool con_z = (
        num1.kind == Kind::unmoveable
        || num2.kind == Kind::unmoveable
        || (num1.z == num2.z)  // for objects that have no length
        || ((num1.z + (num1.length * 1.5)) >= num2.z && num1.z < num2.z)
        || ((num2.z + (num2.length * 1.5)) >= num1.z && num2.z < num1.z)
    );

    return con_x && con_y && con_z;
}

bool off_camera_kill(CharacterSprite & sprite, const Camera & camera) {
    if (sprite.x > camera.boundary().x || (sprite.x + sprite.wide) < camera.x()) {
        kill(sprite);
        return true;
    }
    if (sprite.y > camera.boundary().y || (sprite.y + sprite.high) < camera.y()) {
        kill(sprite);
        return true;
    }
    return false;
}

// From an old function called `killS`, I'm not sure what it's supposed to do.
// It doesn't seem as general as the name would imply and doesn't set enough
// of the fields (unless these are all that's needed to effectively kill a sprite)
void kill(CharacterSprite & sprite) {
    sprite.visible = false;
    sprite.kind = Kind::neutral;
    sprite.name = "";
    sprite.trueVisible = 2;
    sprite.flickerTime = 0;
    sprite.target.release();
    sprite.proc = nullptr;
}

// Makes the sprite seek out it's "seek" vars
void seek(CharacterSprite & s) {
    if (s.x < s.seekx) { s.x = s.x + (s.mph * fs_speed_factor); }
    if (s.x > s.seekx) { s.x = s.x - (s.mph * fs_speed_factor); }
    if (s.y < s.seeky) { s.y = s.y + (s.mph * fs_speed_factor); }
    if (s.y > s.seeky) { s.y = s.y - (s.mph * fs_speed_factor); }
}

// Makes the sprite's size match it's current Animation Frame
void unstretch(CharacterSprite & s) {
    if (s.frame == 0) {
        s.wide = s.srcx2 - s.srcx;
        s.high = s.srcy2 = s.srcy;
    }
    else {
        s.wide = s.Aframe[s.frame].x2 - s.Aframe[s.frame].x;
        s.high = s.Aframe[s.frame].y2 - s.Aframe[s.frame].y;
    }
}

void CharacterSprite::update_jump_physics(double gravity) {
    this->z = this->jump_engine.update(this->z, gravity);
}

void CharacterSprite::start_jump(double jump_magnifier) {
    if (jump_magnifier == 0) {
        jump_magnifier = 1;
    }
    this->jump_engine.start(this->z, this->jump_strength * jump_magnifier);
}

void CharacterSprite::jump_along_with(const CharacterSprite & other) {
    this->jump_engine.jump_along_with(other.jump_engine);
}

bool CharacterSprite::is_falling() const {
    return this->jump_engine.is_falling();
}

JumpRoutine::JumpRoutine()
:   falling(false),
    jump_is_active(false),
    starting_z(0),
    jump_magnifier(1.0),
    timer(0),
    last_z(0)
{
}

void JumpRoutine::jump_along_with(const JumpRoutine & other) {
    this->starting_z = other.starting_z;
    this->jump_magnifier = other.jump_magnifier;
    this->timer = other.timer;
}

void JumpRoutine::start(double starting_z_arg, double jump_magnifier_arg) {
    this->starting_z = starting_z_arg;
    this->last_z = this->starting_z;
    this->timer = 0;
    this->jump_is_active = true;
    this->jump_magnifier = jump_magnifier_arg;
}

double JumpRoutine::update(const double z, const double gravity) {

    double next_z = z;

    if (this->jump_is_active) {
        this->timer += fs_s_per_update;

        next_z = this->starting_z
            + (
            this->jump_magnifier
                * (this->timer * 2)
                )
            - (
                gravity
                * std::pow((this->timer * 2), 2)
                );
        //
        if (next_z < 0) {
            next_z = 0;
            this->jump_is_active = false;
        }
    }
    else {
        //REM------------------------------------------------------
        //  THis is the added gravity that pulls them down   if the're in the ares.
        if (next_z > 0) {
            next_z = next_z - fs_speed_factor;
        }
    }

    this->falling = this->last_z > next_z;
    this->last_z = next_z;
    return next_z;
}

}   }   // end namespace
