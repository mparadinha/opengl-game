#include <iostream>
#include <cmath>

#include "animator.h"

#include "../signals.h"
#include "../entity_pool.h"
#include "../entity_pool_global.h"

#include "../components/animation.h"

Animator::Animator(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing animator..." << std::endl;
}

Animator::~Animator() {}

void Animator::handle_message(message_t msg) {
    switch(msg.code) {
    case NEW_FRAME:
        update_animations(msg.data[0]);
        break;
    }
}

void Animator::update_animations(float dt) {
    std::vector<Entity*> entities = e_pool.query(ANIMATION);
    for(auto e : entities) {
        animation_t* anim = (animation_t*) e->components[ANIMATION];
        continue;

        anim->time = fmod(anim->time + dt, anim->duration);
        pose_t pose = build_pose(anim);
        apply_pose(anim, anim->root_joint, glm::mat4(1), pose);
    }
}

// function that selectes two joint_animation_t's per joint and interpolates between them
// and saves this to a pose_t
pose_t Animator::build_pose(animation_t* animation) {
    pose_t pose(animation->joint_animations.size());
    // TODO: go to every joint_animation and selected the two key frames to interpolate between

    // for now because my test file only has one key frame im just gonna use that
    for(unsigned int i = 0; i < animation->joint_animations.size(); i++) {
        key_frame_t kf = animation->joint_animations[i].key_frames[0];
        glm::vec3 translation = kf.joint_transform.translation;
        glm::quat rotation = kf.joint_transform.rotation;
        pose[i] = glm::translate(glm::mat4(1), translation) * glm::mat4_cast(rotation);
    }

    return pose;
}

// then another one that goes throught the joint_t tree recursively and calculates and
// sets the joint_t transform and also on the animation_t::joint_transforms
void Animator::apply_pose(animation_t* animation, joint_t joint, glm::mat4 parent_bind,
    pose_t pose) {

    // res = joint.trans * parent_bind_trans * joint.inv_bind
    animation->joint_transforms[joint.id] = 
        pose[joint.id] * parent_bind * joint.inverse_bind;
    for(auto& child : joint.children) {
        apply_pose(animation, child, glm::inverse(joint.inverse_bind), pose);
    }
}
