#include <vector>
#include <iostream>
#include <functional>

#include "message_bus.h"
#include "signals.h"

// this system will be notified of any occurences of the codes in the vector
void MessageBus::add_system(std::function<void (message_t)> handler) {
    handlers.push_back(handler);
}

void MessageBus::add_message(message_t message, int priority) {
    if(priority == PRIORITY_NOW) { // message is critical and needs processing right now
        process_message(message);
    }
    else { // just add to the queue for later processing
        queue.push(message);
    }
}

//TODO: use codes for selecting system to notify
void MessageBus::process_message(message_t message) {
    for(auto iter = handlers.begin(); iter != handlers.end(); iter++) {
        (* iter)(message);
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
