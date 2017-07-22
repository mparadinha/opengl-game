#include <vector>
#include <iostream>

#include "message_bus.h"

// this system will be notified of any occurences of the codes in the vector
void MessageBus::add_system(System* system) {
    system_t sys = {system};
    systems.push_back(sys);
}

void MessageBus::add_message(message_t message, int priority = PRIORITY_NORMAL) {
    if(priority == PRIORITY_NOW) { // message is critical and needs processing right now
        process_message(message);
    }
    else { // just add to the queue for later processing
        queue.push(message);
    }
}

void MessageBus::process_message(message_t message) {
    for(system_t sys : systems) {
        sys.system->handle_message(message);
        /* use this to optimize later. right now it's too much of a pain
        for(int code : sys.codes) {
            if(code == message.code) {
                sys.system->handle_message(message);
            }
        }*/
    }
}

// process "everything" in the queue, unless its too much for this frame
void MessageBus::process() {
    // TODO: skip lower priority msg if its too much to process this frame
    while(!queue.empty()) {
        process_message(queue.front());
        queue.pop();
    }
}
