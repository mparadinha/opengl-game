#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "../message_bus.h"

#include "../components/animation.h"

class Animator : public System {
public:
    Animator(MessageBus* msg_bus);
    ~Animator();

    void handle_message(message_t msg);

private:
    void update_animations(float dt);

    pose_t build_pose(animation_t* animation);
    void apply_pose(animation_t* animation, joint_t joint, glm::mat4 parent_bind, pose_t pose);
};

#endif // include guard
