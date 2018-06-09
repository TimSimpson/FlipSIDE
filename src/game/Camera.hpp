#ifndef NND3D_CAMERA_HPP
#define NND3D_CAMERA_HPP
#pragma once

#include <string>
#include <glm/glm.hpp>

namespace nnd3d { namespace game {


// ---------------------------------------------------------------------------
// class Camera
// ---------------------------------------------------------------------------
//     Represents the box currently being looked at by the game.
// ---------------------------------------------------------------------------
class Camera {
public:
    Camera();
    Camera(const Camera & other) = default;

    // Left most coordinate seen by the camera.
    inline int x() const {
        return _ul.x;
    }

    // upper most coordinate seen by camera.
    inline int y() const {
        return _ul.y;
    }

    // upper-left corner
    inline glm::ivec2 ul() const {
        return _ul;
    }

    inline int width() const {
        return _size.x;
    }

    inline int height() const {
        return _size.y;
    }

    inline glm::ivec2 size() const {
        return _size;
    }

    // right most coordinate seen by camera
    inline int x2() const {
        return _ul.x + _size.x;
    }

    // bottom most coordinate seen by camera
    inline int y2() const {
        return _ul.y + _size.y;
    }

    // TODO: split boundary into it's own class.

    // The size before the camera stops (I'm assuming there's also an implicit
    // boundary at 0,0).
    inline glm::ivec2 boundary() const {
        return _boundary;
    }

    void focus(const glm::ivec2 & center, bool enforce_boundary=true);

    void set_boundary(const glm::ivec2 & new_boundary);

    void set_size(const glm::ivec2 & new_size);
private:
    glm::ivec2 _ul;
    glm::ivec2 _size;
    glm::ivec2 _boundary;
};

}   }

#endif
