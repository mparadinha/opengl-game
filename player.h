#ifndef PLAYER_H
#define PLAYER_H

#include "fps_camera.h"

class Player : public FPSCamera {
public:
    Player(float ratio) : FPSCamera(glm::vec3(0), ratio) {};

    ~Player() {};

private:
    glm::vec3 pos;
};

#endif // include guard
