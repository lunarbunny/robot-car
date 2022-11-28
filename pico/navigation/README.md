## Navigation Module

### Introduction

This sub-module library contains:

1. Navigation logic `navigation.c`
2. Mapping Logic `mapping.c`
3. Movement Logic `movement.c`

### Explaination

This library provides a way to map, calculate the shortest path and move in a specified grid.

Definitions in `mappinggod.h`:
```c
#ifndef MAPPINGGOD_H_
#define MAPPINGGOD_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "../motor/motor.h"
#include "../motor/pid.h"
#include "../ultrasonic/ultrasonic.h"

// Related to movement
#define ONEUNIT 19
#define SAFETY_DISTANCE 15

// For mapping and navigation
#define MAZE_SIZE 20 // set to x * y of provided maze
#define MazeConnections 4

// States
#define STATE_IDLE 0		   // State[0]-brb
#define STATE_SRNDIND 1		   // State[1]-Scanning Surroundings
#define STATE_MAPPING 2		   // State[2]-Mapping (choosing direction)
#define STATE_MOVING 3		   // State[3]-Moving
#define STATE_NORMALISING 4	   // State[4]-Mapping Complete, Sorting map struct
#define STATE_COMPUTING_PATH 5 // State[5]-Initial computation of best path
#define STATE_EXECUTING_PATH 6 // State[6]-Giving movement instructions, will alternate between 3 and 6

typedef struct
{
	int coordinates[2];
	int connections[4]; // N, E, S, W
} grid_stats;

typedef struct
{
	grid_stats maze_info[MAZE_SIZE];
	int mapped_count;
	int backstepped;
	int **visited_coords;
	int visited_coords_size;
	int **bot_path;
	int bot_path_size;
	int start_position[2];
	int current_position[2];
} mapping_struct;

typedef struct
{
	int move_count;
	int current_grid[2];
	int ending_grid[2];
	int possible_paths[MAZE_SIZE][MAZE_SIZE][2];
	int actual_path[MAZE_SIZE][2];
} compute_path;

// For MAPPING
// State 1: Scan surroundings
void getTileInfo(mapping_struct *, int *, int *);
// State 2: Choose a direction (shared_buffer contains direction to move in)
void chooseMovement(mapping_struct *, int *, int *);
int checkVisited(mapping_struct *, int);
int checkOpening(mapping_struct *, int);
int getBacktrackDirection(mapping_struct *);
// State 4: Sort mapped maze
void sortMappedMaze(mapping_struct *, int *);

// For NAVIGATION
void enQueue(int, int);
void deQueue();
void findPath(mapping_struct, compute_path *, int *);
int checkDirection(mapping_struct, int[2], int);
int isNextPath(mapping_struct, int[2], int[2]);
void savePath(mapping_struct, compute_path *);

// General
void mallocChecker(void *);
#endif
```

Definitions in `movement.h`:
```c
#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "mappinggod.h"

// State 3 - Moving a unit (uses shared_buffer direction to move)
void moveDirection(mapping_struct *, int *, int *, int *);
// State 6 - Moving all the way (from navigation)
void moveLong(compute_path, int *, int *);

#endif
```
