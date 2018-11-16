#include <iostream>
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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
        apply_pose(anim, anim->root_joint, glm::mat4(1), pose);
    }
}

// function that selectes two joint_animation_t's per joint and interpolates between them
// and saves this to a pose_t
pose_t Animator::build_pose(animation_t* animation) {
    pose_t pose(animation->joint_animations.size());

    for(uint32_t i = 0; i < animation->joint_animations.size(); i++) {
        // find the two keyframes to interpolate between
        auto joint_anim = animation->joint_animations[i];
        uint32_t end = 0;
        while(joint_anim.key_frames[end].time_stamp < animation->time) end++;

        auto start_kf = joint_anim.key_frames[(end - 1) % joint_anim.key_frames.size()];
        auto end_kf = joint_anim.key_frames[end];

        // interpolate translation and rotation
        float blend = (animation->time - start_kf.time_stamp) /
            (end_kf.time_stamp - start_kf.time_stamp);
        if(std::isnan(blend) || std::isinf(blend)) blend = 0;
        glm::vec3 translation = glm::mix(start_kf.joint_transform.translation,
            end_kf.joint_transform.translation, blend);
        glm::quat rotation = glm::mix(start_kf.joint_transform.rotation,
            end_kf.joint_transform.rotation, blend);

        pose[i] = glm::translate(glm::mat4(1), translation) * glm::mat4_cast(rotation);
    }

    return pose;
}

// then another one that goes throught the joint_t tree recursively and calculates and
// sets the joint_t transform and also on the animation_t::joint_transforms
void Animator::apply_pose(animation_t* animation, joint_t joint, glm::mat4 parent_transform,
    pose_t pose) {

    auto transform = parent_transform * pose[joint.id];
    animation->joint_transforms[joint.id] = transform * joint.inverse_bind;

    for(auto& child : joint.children) {
        apply_pose(animation, child, transform, pose);
    }
}
