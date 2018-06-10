#include "Camera.hpp"

namespace nnd3d { namespace game {

Camera::Camera()
:   _ul({0, 0}),
    _size({0, 0}),
    _boundary({0, 0})
{}

void Camera::focus(const glm::ivec2 & center, bool enforce_boundary) {
    _ul = center - (_size / 2);
    // guts.CameraX = center.x - (guts.CameraWidth / 2);
    // guts.CameraY = center.y - (guts.CameraHeight / 2);

    if (enforce_boundary) {
        if (_ul.x < 1) {
            _ul.x = 1;
        }
        if (x2() >= _boundary.x) {
            _ul.x = _boundary.x - 1 - _size.x;
        }
        if (_ul.y < 1) {
            _ul.y = 1;
        }
        if (_ul.y + _size.y >= _boundary.y) {
            _ul.y = _boundary.y - 1 - _size.y;
        }
    }
}

void Camera::set_boundary(const glm::ivec2 & new_boundary) {
    _boundary = new_boundary;
}

void Camera::set_size(const glm::ivec2 & new_size) {
    _size = new_size;
}

}   }
