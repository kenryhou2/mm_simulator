#ifndef SOLVER_H
#define SOLVER_H

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;
typedef struct Coord Coord;
typedef struct Pose Pose;
typedef struct Cell Cell;
typedef struct Floodfill Floodfill;

// typedef struct Mouse
// {
//     char name[20]; //radioflyer2.0
//     Coord mouse_coord;
//     int colPos;
//     int rowPos;   
// } myMouse;
Action solver();
Action leftWallFollower();
int doNextMove();

//extern int manhattan_arr[16][16];




#endif