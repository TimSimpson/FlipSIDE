#include "AnimationFrame.hpp"

namespace nnd3d { namespace view {

AnimationFrame::AnimationFrame()
:   x(0),
    y(0),
    x2(0),
    y2(0)
{
}

void AnimationFrame::set(int arg_x, int arg_y, int arg_x2, int arg_y2) {
    this->x = arg_x;
    this->y = arg_y;
    this->x2 = arg_x2;
    this->y2 = arg_y2;
}

}   }   // end ns
