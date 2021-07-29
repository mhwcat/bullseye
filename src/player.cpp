#include "player.h"
#include <string>
#include "glm/glm.hpp"
#include "reactphysics3d/reactphysics3d.h"

namespace bullseye::entity::player {
    Player::Player(glm::vec3 position, rp3d::Quaternion orientation) : Entity("player", position, orientation, BodyType::RIGID) {

    }

}