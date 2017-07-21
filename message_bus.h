#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <vector>
#include <string>
#include <queue>

enum {
    PRIORITY_NORMAL = 0,
    PRIORITY_NOW
};

typedef struct {
    float x, y, z, w;
} message_data_t;

typedef struct {
    int code;
    message_data_t data;
    // TODO: allow for priority between message in the queue
} message_t;

// base class for all systems that use the messaging system
// this only exists because we need to store pointers to arbitrary systems
class System {
public:
    virtual void handle_message(message_t msg) {};
    std::string name;
};

typedef struct {
    System* system;
    //std::vector<int> codes; // event codes that system cares about <- TODO: use later for optimizing
} system_t;


class MessageBus {
public:
    MessageBus() {};

    void add_system(System* system);
    void add_message(message_t message, int priority); // unless priority is high (and the message needs processing right now) this puts msg in the queue for later handling
    void process(); // process all the messages still in the queue

    ~MessageBus() {};

private:
    void process_message(message_t message);

    std::vector<system_t> systems;
    std::queue<message_t> queue;
};

#endif // include guard
