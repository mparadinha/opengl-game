#include <cmath>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/string_cast.hpp>

#include "physics.h"
#include "../message_bus.h"
#include "../signals.h"
#include "../entity_pool.h"
#include "../entity_pool_global.h"
#include "../components/bounding_volumes.h"
#include "../components/rigid_body.h"
#include "../components/pos_rot_scale.h"

inline bool intersect(const aabb_t& a, const aabb_t& b) {
    for(unsigned int i = 0; i < 3; i++) {
        if(abs(a.center[i] - b.center[i]) > (a.size[i] + b.size[i])) {
            return false; // non collision on any axis implies no collision at all
        }
    }
    return true; // only if all axis collide are the aabb's colliding
}

inline bool intersect(const sphere_bv_t& a, const sphere_bv_t& b) {
    glm::vec3 diff = a.center - b.center; // different vector
    float dist_sq = glm::dot(diff, diff); // square distance between centers 
    float radius = a.radius + b.radius;
    // squaring this is cheaper than doing the square root of dist_sq
    return dist_sq < radius * radius;
}

Physics::Physics(MessageBus* msg_bus) : System(msg_bus) {
    std::cout << "initing physics..." << std::endl;
}

void Physics::handle_message(message_t msg) {
    switch(msg.code) {
    case NEW_FRAME:
        update(msg.data[0]);
        break;
    }
}

void Physics::update(float dt) {
    std::cout << "physics::update::dt = " << dt << std::endl;
    glm::vec3 g(0, -9.8, 0);

    std::vector<Entity*> bodies = e_pool.query(RIGID_BODY | AABB);
    bodies.push_back(e_pool.get_special(CAMERA));
    for(unsigned int i = 0; i < bodies.size(); i++) {
        Entity* body = bodies[i];

        rigid_body_t* rb = (rigid_body_t*) body->components[RIGID_BODY];
        // floating objects aren't affected by gravity
        if(rb->floating) {
        }

        //glm::vec3 accel = g;
        glm::vec3 accel = glm::vec3(0);

        rb->pos += rb->vel * dt;
        rb->vel += accel * dt;

        // check for nan
        /*
        std::cout << "checking for nan. rb->pos: ";
        for(int i = 0; i < 3; i++) {
            std::cout << rb->pos[i] << ", ";
            assert(!std::isnan(rb->pos[i]));
        }
        std::cout << "\n";
        */

        if((body->bitset & CAMERA) == CAMERA) {
            std::cout << "physics:: updating camera's rb\n";
            std::cout << "rb->pos: " << glm::to_string(rb->pos) << std::endl;
            std::cout << "rb->vel: " << glm::to_string(rb->vel) << std::endl;
        }

        aabb_t* aabb = (aabb_t*) body->components[AABB];
        aabb->center += rb->vel * dt;

        // also update the position in the prs if the entity has one
        if((body->bitset & POS_ROT_SCALE) == POS_ROT_SCALE) {
            pos_rot_scale_t* prs = (pos_rot_scale_t*) body->components[POS_ROT_SCALE];
            prs->pos = rb->pos;
        }

        // check for collisions
        if(i == bodies.size() - 1) continue; // last object has already been checked for collision
        for(unsigned int j = i + 1; j < bodies.size(); j++) {
            Entity* other = bodies[j];
            aabb_t* other_aabb = (aabb_t*) other->components[AABB];
            rigid_body_t* other_rb = (rigid_body_t*) other->components[RIGID_BODY];
            if(intersect(*aabb, *other_aabb)) {
                // hack below, when the camera/player system is better implemented
                // this should be changed
                // if colliding against camera just stop the camera from moving
                if((other->bitset & CAMERA) == CAMERA) {
                    other_rb->vel = glm::vec3(0);
                    continue;
                }

                rb->pos -= rb->vel * dt;
                // every collision is perfectly elastic in the real world.
                // even friction
                rb->vel *= -1; //TODO: only switch vel component where the collision occured
                other_rb->vel *= -1;
            }
        }
    }
}
