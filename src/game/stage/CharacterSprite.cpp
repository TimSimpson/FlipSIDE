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
        (num1.kind == Kind::unmoveable || num2.kind == Kind::unmoveable)
        || ((num1.z + (num1.length * 1.5)) >= num2.z && num1.z < num2.z)
        || ((num2.z + num2.length * 1.5) >= num1.z && num2.z < num1.z)
    );

    return con_x && con_y && con_z;
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
