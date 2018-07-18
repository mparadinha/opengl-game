#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>

#include "../message_bus.h"
#include "../entity_pool.h"
#include "../components/bounding_volumes.h"

struct collision_t {
    Entity* a;
    Entity* b;
    float penetration;
    glm::vec3 normal;
};

bool intersect(const aabb_t& a, const aabb_t& b);
bool intersect(const sphere_bv_t& a, const sphere_bv_t& b);

collision_t collision_info(Entity* a, Entity* b);

class Physics : public System {
public:
    Physics(MessageBus* msg_bus);
    ~Physics() {};

    void handle_message(message_t msg);

    void resolve_collision(collision_t collision);

private:
    void update(float dt);
};

#endif
