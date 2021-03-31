#ifndef BULLSEYE_MATH_UTILS_H
#define BULLSEYE_MATH_UTILS_H

#include <glm/glm.hpp>

#define PI_VAL 3.141592653589793238f

namespace bullseye::math_utils {
    inline float to_radians(float angle) {
        return (angle * ((float) PI_VAL / 180.f));
    }

    inline glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return (x * (1.f - t)) + (y * t);
    }
}

#endif