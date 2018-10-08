#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct joint_transform_t {
    // this translation and rotation is in respect to the parent of this joint
    glm::vec3 translation;
    glm::quat rotation;
};

struct key_frame_t {
    std::vector<joint_transform_t> joint_tranforms;
    float time_stamp;
};

struct joint_animation_t {
    std::vector<key_frame_t> key_frames;
};

// this joint struct represent the joint at the current pose
struct joint_t {
    std::vector<joint_t> children;
    unsigned int id; // index into the final array of joint transforms
    // that are sent to the shader
    glm::mat4 transform; // transforms the joint from the bind pose, its default
    // state, (i.e no animation applied) into the current pose
    std::string name;
    glm::mat4 inverse_bind; // used by the animation system to calculate the
    // transform. it is in model space and in the default pose (hence the bind) 
};

struct animation_t {
    float duration;
    std::map<std::string, joint_animation_t> joint_animations;
    joint_t root_joint; // this tree of joints is what is updated every frame

    // this vector contains the final, shader ready matrix transforms for the current
    // pose. this is the array that the renderer should send to the shaders.
    // this informations is already on the joint tree, this is simply for convenience
    std::vector<glm::mat4> joint_transforms;
};

#endif // include guard
