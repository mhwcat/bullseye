#ifndef BULLSEYE_MATH_H
#define BULLSEYE_MATH_H

#include <cmath>
#include <glm/glm.hpp>

namespace bullseye::structs {
    struct Vec3f {
        float x;
        float y;
        float z;
    };

    struct Vec4f {
        float x;
        float y;
        float z;
        float w;
    };

    struct Vec3u32 {
        u_int32_t x;
        u_int32_t y;
        u_int32_t z;
    };

    inline float to_radians(float angle) {
        return (angle * (M_PI / 180.0f));
    }

    inline glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
        return x * (1.f - t) + y * t;
    }
}


#endif