#ifndef PHYSICS_H
#define PHYSICS_H

#include "../message_bus.h"
#include "../components/bounding_volumes.h"

inline bool intersect(const aabb_t& a, const aabb_t& b);
inline bool intersect(const sphere_bv_t& a, const sphere_bv_t& b);

class Physics : public System {
public:
    Physics(MessageBus* msg_bus) : System(msg_bus) {};
    ~Physics() {};

    void handle_message(message_t msg);

private:
    void update(float dt);
};

#endif
