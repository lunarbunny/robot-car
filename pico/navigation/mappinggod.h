#ifndef MAPPINGGOD_H_
#define MAPPINGGOD_H_

//////////////////////////////////////////////////
//[navigation header]
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../motor/motor.h"
#include "../motor/pid.h"

// For movement sensing
#define ONEUNIT 27

// For mapping and navigation
#define BUFFER_SIZE 16
#define MAZE_SIZE 20 // set to x * y of provided maze
#define MazeConnections 4

// For mapping and navigation
typedef struct
{
	int coordinates[2];
	int connections[4]; // N, E, S, W
} grid_stats;

// For mapping
void populateProvidedMaze(grid_stats[MAZE_SIZE]);
void mallocChecker(void *);

// For navigation
void allTraversablePaths(grid_stats[MAZE_SIZE]);
int checkDirection(grid_stats[MAZE_SIZE], int, int, int);
int findPath(grid_stats[MAZE_SIZE], int[2], int[2], int[MAZE_SIZE][MAZE_SIZE][2], int[5][4]);
int isNextPath(grid_stats[MAZE_SIZE], int[2], int[2]);
void savePath(grid_stats[MAZE_SIZE], int, int[MAZE_SIZE][2], int[MAZE_SIZE][MAZE_SIZE][2], int[2]);
void enQueue(int, int);
void deQueue();

//////////////////////////////////////////////////
//[mapping header]

#include <stdlib.h>
#include "../ultrasonic/ultrasonic.h"

#define MAZE_SIZE 20 // set to x * y of provided maze

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

void getTileInfo(mapping_struct *, int *);
int chooseMovement(mapping_struct *);
int checkVisited(mapping_struct *, int);
int checkOpening(mapping_struct *, int);
void *sortMappedMaze(mapping_struct *, int *);
void mallocChecker(void *);

#endif
