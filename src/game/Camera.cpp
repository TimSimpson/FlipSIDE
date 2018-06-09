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

void Camera::focus(const glm::ivec2 & center, bool enforce_boundary) {
    guts.CameraX = center.x - (guts.CameraWidth / 2);
    guts.CameraY = center.y - (guts.CameraHeight / 2);

    if (enforce_boundary) {
        if (guts.CameraX < 1) {
            guts.CameraX = 1;
        }
        if (x2() >= guts.cameraStopX) {
            guts.CameraX = guts.cameraStopX - 1 - guts.CameraWidth;
        }
        if (guts.CameraY < 1) {
            guts.CameraY = 1;
        }
        if (guts.CameraY + guts.CameraHeight >= guts.cameraStopY) {
            guts.CameraY = guts.cameraStopY - 1 - guts.CameraHeight;
        }
    }
}

void Camera::set_boundary(const glm::ivec2 & new_boundary) {
    guts.cameraStopX = new_boundary.x;
    guts.cameraStopY = new_boundary.y;
}

void Camera::set_size(const glm::ivec2 & new_size) {
    guts.CameraWidth = new_size.x;
    guts.CameraHeight = new_size.y;
}

}   }
