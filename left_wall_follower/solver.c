#include "solver.h"
#include "API.h"

Action solver() {
    return leftWallFollower();
}

Action leftWallFollower() {
    if(API_wallFront()) {
        if(API_wallLeft()){
            return RIGHT;
        }
        return LEFT;
    }
    return FORWARD;
}

Action floodFill() {
    return IDLE;
}