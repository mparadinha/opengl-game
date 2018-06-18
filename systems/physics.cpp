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

    // update all the positions and velocities
    glm::vec3 accel(0, 0, 0);
    for(Entity* body : bodies) { 
        rigid_body_t* rb = (rigid_body_t*) body->components[RIGID_BODY];
        rb->pos += rb->vel * dt;
        rb->vel += accel * dt;

        aabb_t* aabb = (aabb_t*) body->components[AABB];
        aabb->center += rb->vel * dt;

        // also update the position in the prs if the entity has one
        if((body->bitset & POS_ROT_SCALE) == POS_ROT_SCALE) {
            pos_rot_scale_t* prs = (pos_rot_scale_t*) body->components[POS_ROT_SCALE];
            prs->pos = rb->pos;
        }
    }

    // create all the collision info
    std::vector<collision_t> collisions;
    for(unsigned int i = 0; i < bodies.size() - 1; i++) {
        for(unsigned j = i; j < bodies.size(); j++) {    
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
}

void Physics::resolve_collision(collision_t collision) {
    // get the components that are going to be changed
    rigid_body_t* a = (rigid_body_t*) collision.a->components[RIGID_BODY];
    rigid_body_t* b = (rigid_body_t*) collision.b->components[RIGID_BODY];

    // relative velocity along the contact normal
    float vel_normal = glm::dot((b->vel - a->vel), collision.normal);

    // use the lowest restitution value
    // HACK BELOW: using a hard coded value for restitution for now while its not implemented
    // on the rigid_body component
    // TODO: have a restitution value on each rigib_body component (related to the object
    // material)
    float restitution = 0.4;

    // TODO: keep an inverse mass value on the rigib_body to avoid recomputation
    // HACK: hard coded calc for inverse mass (1/mass) that should be done when
    // creating the component and based on a density * volume (that would be different
    // various types of materials)
    float density = 1;
    float inv_mass_a = 1 / (density * (a->scale.x * a->scale.y * a->scale.z * 8));
    float inv_mass_b = 1 / (density * (b->scale.x * b->scale.y * b->scale.z * 8));

    // calculate the magnitude of the impulse to be applied
    // (magic formula from: https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331)
    float impulse_abs = -(1 + restitution) * vel_normal / (inv_mass_a + inv_mass_b);

    // apply the impulse
    glm::vec3 impulse = impulse_abs * collision.normal;
    a->vel -= inv_mass_a * impulse;
    b->vel += inv_mass_b * impulse;
}

collision_t collision_info(Entity* a, Entity* b) {
    collision_t collision = {a, b, 0, {0, 0, 0}};

    // fetch bounding box components for checking collisions and
    // rigid body for velocities
    aabb_t* a_aabb = (aabb_t*) a->components[AABB];
    aabb_t* b_aabb = (aabb_t*) b->components[AABB];

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
        for(unsigned int i = 0; i < 3; i++) {
            if(collision.penetration == depth[i]) {
                collision.normal[i] = 1;
            }
        }
    }

    return collision;
}
