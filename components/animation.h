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
    joint_transform_t joint_transform;
    float time_stamp;
};

// in a more advanced system the translation and rotation animations might have
// keyframes at different times and so we would need to have two separate key_frame
// arrays, one for the translations key frames and one for the rotation key frames
struct joint_animation_t {
    std::vector<key_frame_t> key_frames; // these are not necessarily ordered in time
};

// this joint struct represent the joint at the current pose
struct joint_t {
    std::vector<joint_t> children;
    unsigned int id; // index into the final array of joint transforms
    // that are sent to the shader
    glm::mat4 transform; // transforms the joint from the bind pose, its default
    // state, (i.e no animation applied) into the current pose. relative to parent
    std::string name;
    glm::mat4 inverse_bind; // used by the animation system to calculate the
    // transform. it is in model space and in the default pose (hence the bind) 
};

typedef std::vector<glm::mat4> pose_t; // all these glm::mat4 are relative to the parent

struct animation_t {
    float duration;
    float time; // for keeping track of time
    std::vector<joint_animation_t> joint_animations; // joint_animations[0] is the
    // animations of the joint_t whose id = 0;
    joint_t root_joint; // this tree of joints is what is updated every frame

    // this vector contains the final, shader ready matrix transforms for the current
    // pose. this is the array that the renderer should send to the shaders.
    // this informations is already on the joint tree, this is simply for convenience
    std::vector<glm::mat4> joint_transforms;
};

#endif // include guard
