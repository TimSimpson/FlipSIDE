#include "CinemaType.hpp"

namespace nnd3d { namespace game {

CinemaType::CinemaType()
:   frame1(0),
    frame2(0),
    frame3(0),
    frame4(0),
    color1(0),
    color2(0),
    color3(0),
    color4(0),
    wavefile(),
    miscTime(0)
{
}

CinemaType::CinemaType(
    const int _frame1, const int _frame2,
	const int _frame3, const int _frame4, const int _color1,
	const int _color2, const int _color3, const int _color4,
	const std::string & _wavefile, const double _miscTime)
{
	this->frame1 = _frame1;
	this->frame2 = _frame2;
	this->frame3 = _frame3;
	this->frame4 = _frame4;
	this->color1 = _color1;
	this->color2 = _color2;
	this->color3 = _color3;
	this->color4 = _color4;
	this->wavefile = _wavefile;
	this->miscTime = _miscTime;
}

}   }   // end namespace
