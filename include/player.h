#ifndef BULLSEYE_PLAYER_H
#define BULLSEYE_PLAYER_H

#include "glm/glm.hpp"
#include "reactphysics3d/reactphysics3d.h"

#include "entity.h"

namespace bullseye::entity::player {
    class Player : public Entity {
    public:
        Player(glm::vec3 position, rp3d::Quaternion orientation);
    };
}
#endif