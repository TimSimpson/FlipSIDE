#ifndef NND3D_CHARACTERSPRITE_HPP
#define NND3D_CHARACTERSPRITE_HPP
#pragma once

#include <array>
#include <string>
#include <lp3/core.hpp>
#include <lp3/gfx.hpp>
#include "../AnimationFrame.hpp"
#include "../Vertex.hpp"

namespace nnd3d { namespace game {

class CharacterProc;

struct CharacterSprite
{
     // This initializer just zeroes everything out the way it would have been
     // in Visual Basic.
     CharacterSprite();

     double x; //Integer
     double y; //Integer
     double lastX;
     double lastY;
     double z;  //2017: Changed to double to prevent some bugs
     double wide; //Integer 'width and height
     double high; //Integer
     int length;
     std::string dir;

     int srcx;
     int srcy;
     int srcx2;
     int srcy2;

     std::array<view::AnimationFrame, 20> Aframe;
     int seekx;
     int seeky;
     double speed;  //this is tricky.  Every time this amount of seconds pass, its little routine is updated.
     double time; // this marks the time until the sprite can do its thing again.
     int mph; //refers to how many pixels the thing can haul
     int texture;

     bool visible;
     int parent; //whose their parent sprite (what is the reason for their existence?) are they a fireball cast by sprite 0, perhaps
     std::array<Vertex, 4> SpriteVerts;

     //Location As D3DVECTOR2
     int frame;
     std::string name;

     double hp; //hp!
     double mhp;

     int jumpStart;  //These are for jumping
     int jumpStrength;
     double jumpTime;
     int lastJump;
     int multiJump;
     int maxJump; //max # of jumps

     double flickerTime; //Lets them flicker until then
     bool flickOn;
     int trueVisible;
     double invTime; //how many seconds they flicker when hit

     std::string mode; //which mode are they in?
     int kind; //if kind is 0, they are neutral.  If 1, good guy.  If 2, bad guy, if 3 , good guy weapon...
     std::string deathType; //how do they die?
     double miscTime; // they must wait this long to do misc. actions
     glm::vec4 color; //lets throw some color into the mix

     std::string soundFile; //sound files

     int zOrder; //higher numbers are drawn last.
     bool drawTrue; //used in the draw ordering subprogram

     bool mover;

     bool reverse; //to flip bitmap or not TRUE for transposing from left to right
     int target; // it is who they are attacking

     double jumpM;

     // Behavior of this sprite.
     CharacterProc * proc;
};

void unstretch(CharacterSprite & s);

}    }

#endif
