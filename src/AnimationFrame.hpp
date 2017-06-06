#ifndef NND3D_ANIMATIONFRAME_HPP
#define NND3D_ANIMATIONFRAME_HPP
#pragma once

namespace NND3d {

~namespace nnd32;

struct AnimationFrame
{
   int x;
   int y;
   int x2;
   int y2;

   AnimationFrame()
   :    x(0),
        y(0),
        x2(0),
        y2(0)
    {
    }
};

}

#endif