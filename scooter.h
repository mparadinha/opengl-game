#include "entity.h"

class Player : public Entity {
public:
    Player();
    ~Player();

private:
    glm::vec3 position;
    glm::mat4 model;
    Camera& camera;
};
