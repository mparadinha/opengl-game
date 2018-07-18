#include <cmath>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/extended_min_max.hpp>

#include "physics.h"
#include "../message_bus.h"
#include "../signals.h"
#include "../entity_pool.h"
#include "../entity_pool_global.h"
#include "../components/bounding_volumes.h"
#include "../components/rigid_body.h"
#include "../components/pos_rot_scale.h"

bool intersect(const aabb_t& a, const aabb_t& b) {
    for(unsigned int i = 0; i < 3; i++) {
        if(abs(a.center[i] - b.center[i]) > (a.size[i] + b.size[i])) {
            return false; // non collision on any axis implies no collision at all
        }
    }
    return true; // only if all axis collide are the aabb's colliding
}

bool intersect(const sphere_bv_t& a, const sphere_bv_t& b) {
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
    // we update, check for collision and resolve collision all in
    // separate steps because the resolution of a collision could make
    // it so another collision, which should be proccessed, dissapears

    // get all entities to process
    std::vector<Entity*> bodies = e_pool.query(RIGID_BODY | AABB);
    bodies.push_back(e_pool.get_special(CAMERA));

    // update velocities
    glm::vec3 accel(0, -9.8, 0);
    for(Entity* body : bodies) {
        rigid_body_t* rb = (rigid_body_t*) body->components[RIGID_BODY];

        if(rb->floating) continue;

        // update velocity
        rb->vel += accel * dt;
    }

    // create all the collision info
    std::vector<collision_t> collisions;
    for(unsigned int i = 0; i < bodies.size() - 1; i++) {
        for(unsigned int j = i + 1; j < bodies.size(); j++) {
            collision_t info = collision_info(bodies[i], bodies[j]);
            if(info.penetration < 0) {
                collisions.push_back(info);
            }
        }
    }

    // resolve all the collisions by applying impulses to objects
    for(collision_t collision : collisions) {
        resolve_collision(collision);
    }

    // update positions
    for(Entity* body : bodies) {
        rigid_body_t* rb = (rigid_body_t*) body->components[RIGID_BODY];
        aabb_t* aabb = (aabb_t*) body->components[AABB];

        if(rb->floating) continue;

        // update position
        rb->pos += rb->vel * dt;
        aabb->center += rb->vel * dt;

        // also update the position in the prs if the entity has one
        if((body->bitset & POS_ROT_SCALE) == POS_ROT_SCALE) {
            pos_rot_scale_t* prs = (pos_rot_scale_t*) body->components[POS_ROT_SCALE];
            prs->pos = rb->pos;
        }
    }
}

void Physics::resolve_collision(collision_t collision) {
    // get the components that are going to be changed
    rigid_body_t* a = (rigid_body_t*) collision.a->components[RIGID_BODY];
    rigid_body_t* b = (rigid_body_t*) collision.b->components[RIGID_BODY];

    // no need to resolve collision between two immovable objects
    if(a->inv_mass == 0 && b->inv_mass == 0) return;

    // relative velocity along the contact normal
    float vel_normal = glm::dot((b->vel - a->vel), collision.normal);

    // if the objects will be separated in the next time step don't bother resolving
    if(vel_normal > 0) return;

    // use the lowest restitution value
    float restitution = glm::min(a->material.restitution, b->material.restitution);

    // calculate the magnitude of the impulse to be applied
    // (magic formula from: https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331)
    float impulse_abs = -(1 + restitution) * vel_normal / (a->inv_mass + b->inv_mass);

    // apply the impulse
    glm::vec3 impulse = impulse_abs * collision.normal;
    a->vel -= a->inv_mass * impulse;
    b->vel += b->inv_mass * impulse;

    // apply static friction
    // we need to recalculate the relative velocity _vector_
    glm::vec3 rel_vel = b->vel - a->vel;
    // find the tangent direction
    glm::vec3 tangent = rel_vel - (glm::dot(rel_vel, collision.normal)) * collision.normal;
    if(glm::length(tangent) != 0) tangent = glm::normalize(tangent);
    // find the magnitude of our friction impulse
    float friction_mag = -glm::dot(rel_vel, tangent) / (a->inv_mass + b->inv_mass);
    // clamp friction to be below mu * normal_force (coulomb's law)
    float mu = (a->material.drag_coef + b->material.drag_coef) / 2;
    friction_mag = (abs(friction_mag) <= mu * impulse_abs) ? friction_mag : -mu * impulse_abs;
    // finally apply the friction impulse
    glm::vec3 friction_impulse = friction_mag * tangent;
    a->vel -= a->inv_mass * friction_impulse;
    b->vel += b->inv_mass * friction_impulse;

    // correct positions (this stops objects from sinking into the ground
    // or jittering in place)
    float percent = 0.2; // percentage of penetration that is corrected
    float allowed = 0.05; // percentage of penetration that is allowed to occur
    glm::vec3 correction = (std::max(collision.penetration - allowed, 0.0f) / (a->inv_mass + b->inv_mass)) * percent * collision.normal;
    a->pos -= a->inv_mass * correction;
    b->pos -= b->inv_mass * correction;
}

collision_t collision_info(Entity* a, Entity* b) {
    collision_t collision = {a, b, 0, {0, 0, 0}};

    // fetch bounding box components for checking collisions and
    // rigid body for positions/velocities
    aabb_t* a_aabb = (aabb_t*) a->components[AABB];
    aabb_t* b_aabb = (aabb_t*) b->components[AABB];
    rigid_body_t* a_rb = (rigid_body_t*) a->components[RIGID_BODY];
    rigid_body_t* b_rb = (rigid_body_t*) b->components[RIGID_BODY];

    // calculate penetration depth in each direction
    glm::vec3 depth;
    for(unsigned int i = 0; i < 3; i++) {
        depth[i] = abs(a_aabb->center[i] - b_aabb->center[i]) -
            (a_aabb->size[i] + b_aabb->size[i]);
    }

    // calculate value of least penetration. because the depth should be
    // negative on all axis if colliding, this correspondes to the biggest value
    collision.penetration = glm::max(depth.x, depth.y, depth.z);

    // calculate the contact normal (or don't bother if not colliding)
    if(collision.penetration < 0) {
        // TODO: think of a better way to calculate contact normal
        glm::vec3 diff = b_rb->pos - a_rb->pos;
        for(unsigned int i = 0; i < 3; i++) {
            if(collision.penetration == depth[i]) {
                collision.normal[i] = diff[i] > 0 ? 1 : -1;
            }
        }
    }

    return collision;
}

