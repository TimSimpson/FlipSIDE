#include "CharacterSprite.hpp"
#include "../Game.hpp"

namespace nnd3d { namespace game {

std::istream & operator >>(std::istream & in, Kind & kind) {
    int original_int;
    in >> original_int;
    kind = static_cast<Kind>(original_int);
    return in;
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
    jumpStart(0),
    jumpStrength(0),
    jumpTime(0),
    lastJump(0),
    multiJump(0),
    maxJump(0),
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
    target(0),
    jumpM(0),
    proc()
{
}


double getProx(CharacterSprite & who, CharacterSprite & who2) {
    // Finds who is closet
    return std::abs(who.getMiddleX() - who2.getMiddleX())
        + std::abs(who.getMiddleY() - who.getMiddleY());
    // amount of pixels they are close
}

CollisionStatus hit_detection(CharacterSprite num1, CharacterSprite num2,
    bool whatKind) {

    bool con1 = false;
    bool con2 = false;
    bool con3 = false;

    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    // did num1 touch num2 from the left of num2?
    if ((num1.x + num1.wide) >= num2.x && num1.x < num2.x) {
        con1 = true;
        left = true;
    }
    // did num1 touch num2 from the right?
    if ((num2.x + num2.wide) >= num1.x && num2.x < num1.x) {
        con1 = true;
        right = true;
    }

    // did num1 touch num2 from the North?
    if ((num1.y + num1.high) >= num2.y && num1.y < num2.y) {
        con2 = true;
        up = true;
    }
    // did num1 touch num2 from the South?
    if ((num2.y + num2.high) >= num1.y && num2.y < num1.y) {
        con2 = true;
        down = true;
    }

    // If the Kind is unmoveable, assume it is infinitly tall and don't bother
    // checking the Z dimension
    if (num1.kind == Kind::unmoveable || num2.kind == Kind::unmoveable) {
        con3 = true;
    }
    else
    {
        // do the same thing for the Z dimension. Don't bother remembering
        // if they were above or below
        if ((num1.z + (num1.length * 1.5)) >= num2.z && num1.z < num2.z) {
            con3 = true;
        }
        if (num1.z == num2.z) { con3 = 1; }
        //Rem- Added as of 11/27/00
        if ((num2.z + num2.length * 1.5) >= num1.z && num2.z < num1.z) {
            con3 = true;
        }
    }

    // UPDATE: this is a dualistic function. Based on what kind, the old code
    //         would determine if there had been a collision, but then throw
    //         that away and return the following. SO I moved the code that
    //         returns `collided` to below here, this isn't a bug.
    //         TODO: Split this into two functions!
    if (whatKind == true) {
        if (left == 1 && up == 1) { return CollisionStatus::aligned_from_ul; }
        if (left == 1 && down == 1) { return CollisionStatus::aligned_from_bl; }
        if (right == 1 && up == 1) { return CollisionStatus::aligned_from_ur; }
        if (right == 1 && down == 1) { return CollisionStatus::aligned_from_br; }
        if (left == 1) { return CollisionStatus::aligned_from_left; }
        if (right == 1) { return CollisionStatus::aligned_from_right; }
        if (up == 1) { return CollisionStatus::aligned_from_top; }
        if (down == 1) { return CollisionStatus::aligned_from_bottom; }
    }

    // 1 seems to mean "they hit it, who cares how"
    if (con1 && con2 && con3) { return CollisionStatus::collided; }

    return CollisionStatus::not_aligned;
}

bool off_camera_kill(CharacterSprite & sprite, Camera & camera) {
    if (sprite.x > camera.x() + 640 || (sprite.x + sprite.wide) < camera.x()) {
        kill(sprite);
        return true;
    }
    if (sprite.y > camera.y() + 480 || (sprite.y + sprite.high) < camera.y()) {
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
    sprite.target = -1;
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

}   }   // end namespace
