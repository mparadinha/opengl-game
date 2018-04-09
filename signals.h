#ifndef SIGNALS_H
#define SIGNALS_H

enum {
    PLAYER_MOVE,

    //TODO: remove these after first person camera has been
    // implemented with the new entity system
    MOVE_BACK, MOVE_FRONT, MOVE_LEFT, MOVE_RIGHT,
    STOP_MOVE_BACK, STOP_MOVE_FRONT, STOP_MOVE_LEFT, STOP_MOVE_RIGHT,

    MOUSE_MOVE,
    MOUSE_SCROLL,

    SWITCH_CAMERA_STYLE,

    NEW_FRAME
};

#endif // include guard
