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

        anim->time = fmod(anim->time + dt, anim->duration);
        pose_t pose = build_pose(anim);
        apply_pose(anim, anim->root_joint, pose);
    }
}

// function that selectes two joint_animation_t's per joint and interpolates between them
// and saves this to a pose_t
pose_t Animator::build_pose(animation_t* animation) {
    pose_t pose(animation->joint_animations.size());
    // go to every joint_animation and selected the two key frames to interpolate between
    
    return pose;
}

// then another one that goes throught the joint_t tree recursively and calculates and
// sets the joint_t transform and also on the animation_t::joint_transforms
void Animator::apply_pose(animation_t* animation, joint_t joint, pose_t pose) {
}
