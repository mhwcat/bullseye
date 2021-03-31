#ifndef BULLSEYE_MATH_UTILS_H
#define BULLSEYE_MATH_UTILS_H

#define _USE_MATH_DEFINES
#include <cmath>

#include <glm/glm.hpp>

namespace bullseye::math_utils {
    inline float to_radians(float angle) {
        return (angle * ((float) M_PI / 180.f));
    }

    inline glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return (x * (1.f - t)) + (y * t);
    }
}

#endif