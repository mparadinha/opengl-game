#ifndef MESSAGE_BUS_H
#define MESSAGE_BUS_H

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <functional>

enum {
    PRIORITY_NORMAL = 0,
    PRIORITY_NOW
};

#define MAX_ARGS 8
typedef struct message_t {
    int code;
    float data[MAX_ARGS];
    // TODO: allow for priority between message that are in the queue
} message_t;

class MessageBus {
public:
    MessageBus() {};

    void add_system(std::function<void (message_t)>);
    void add_message(message_t message, int priority = PRIORITY_NORMAL); // unless priority is high (and the message needs processing right now) this puts msg in the queue for later handling
    void process(); // process all the messages still in the queue

    ~MessageBus() {};

private:
    void process_message(message_t message);

    std::vector<std::function<void (message_t)>> handlers;
    std::queue<message_t> queue;
};

// base class for all systems that use the messaging system
class System {
public:
    System(MessageBus* msg_bus) {
        this->msg_bus = msg_bus;
        std::cout << "[system] init: adding self to msg_bus\n";
        this->msg_bus->add_system(this->get_handle_func());
    }

    //std::vector<int> codes; for later

protected:
    std::function<void (message_t)> get_handle_func() {
        // create a lambda function for the handler
        auto message_listener = [=](message_t msg) -> void {
            this->handle_message(msg);
        };
        return message_listener;
    }

    void send_msg(message_t msg, int priority = PRIORITY_NORMAL) {
        msg_bus->add_message(msg, priority);
    }

    virtual void handle_message(message_t msg) {};

    MessageBus* msg_bus;
};

#endif // include guard
