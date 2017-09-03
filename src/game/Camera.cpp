#include "Camera.hpp"

namespace nnd3d { namespace game {

CameraGuts::CameraGuts()
:   CameraX(0),
    CameraY(0),
    CameraWidth(0),
    CameraHeight(0),
    cameraStopX(0),
    cameraStopY(0)
{}


Camera::Camera(CameraGuts & _guts)
:   guts(_guts)
{}

}   }
