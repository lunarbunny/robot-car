#include "mappinggod.h"

int xCoords[MAZE_SIZE], yCoords[MAZE_SIZE], rear = -1; // For navigation

// Check all coordinates' traversable paths
void allTraversablePaths(grid_stats mazeSetup[MAZE_SIZE]) {
	// Check all coordinates
	for (int mazeSize = 0; mazeSize < MAZE_SIZE; mazeSize++) {
		int intDirection = 0;

		// Coordinate go N -> E -> S -> W
		for (int currDirection = 0; currDirection < 4; currDirection++) {
			// Check if direction is connected
			if (mazeSetup[mazeSize].connections[currDirection] == 1) {

				printf("Grid: (%d, %d) & intDirection = %d\n",
					mazeSetup[mazeSize].coordinates[0], mazeSetup[mazeSize].coordinates[1], intDirection);
			}
			intDirection++;
		}
		intDirection = 0;
	}
}

// I guess its a BFS check to find path from starting grid to ending grid
int findPath(grid_stats mazeSetup[MAZE_SIZE], int currentGrid[2], int endingGrid[2], int possiblePaths[MAZE_SIZE][MAZE_SIZE][2], int visitedCoords[5][4]) {
	int grids_in_next_layer = 0; // The number of grids in the next layer of nodes to check
	int grids_in_current_layer = 0; // The current layer's number of nodes to check
	int numOfGrids = 0; // Number of grids to traverse
	int current_layer = 0; // Current layer of nodes for possible paths

	enQueue(currentGrid[0], currentGrid[1]); // Queue the starting coords

	while(rear >= 0) {
		currentGrid[0] = xCoords[0]; // Current X
		currentGrid[1] = yCoords[0]; // Current Y
		visitedCoords[currentGrid[0]][currentGrid[1]]  = 1; // Mark the current grid as visited

		// Check if there are no more grids in current layer 
		if (grids_in_current_layer == 0) {
			grids_in_current_layer = grids_in_next_layer;
			grids_in_next_layer = 0;
			
			if(numOfGrids != 0) {
				possiblePaths[numOfGrids - 1][current_layer][0] = '\n';
				possiblePaths[numOfGrids - 1][current_layer][1] = '\n';
			}
			
			numOfGrids++;
			current_layer = 0;
		}

		possiblePaths[numOfGrids - 1][current_layer][0] = xCoords[0];
		possiblePaths[numOfGrids - 1][current_layer][1] = yCoords[0];
		current_layer++;
		deQueue(); // Remove current grid from the queue

		// Check if current grid is destination
		if (currentGrid[0] == endingGrid[0] && currentGrid[1] == endingGrid[1]) {
			possiblePaths[numOfGrids - 1][current_layer][0] = '\n';
			possiblePaths[numOfGrids - 1][current_layer][1] = '\n';
			return numOfGrids;
		}

		// Check all possible grids that you can traverse to from current grid
		if(checkDirection(mazeSetup, currentGrid[0], currentGrid[1], 0) && !visitedCoords[currentGrid[0]][currentGrid[1] - 1]) {
			enQueue(currentGrid[0], currentGrid[1] - 1);
			grids_in_next_layer++;
		}
		if(checkDirection(mazeSetup, currentGrid[0], currentGrid[1], 1) && !visitedCoords[currentGrid[0] + 1][currentGrid[1]]) {
			enQueue(currentGrid[0] + 1, currentGrid[1]);
			grids_in_next_layer++;
		}
		if(checkDirection(mazeSetup, currentGrid[0], currentGrid[1], 2) && !visitedCoords[currentGrid[0]][currentGrid[1] + 1]) {
			enQueue(currentGrid[0], currentGrid[1] + 1);
			grids_in_next_layer++;
		}
		if(checkDirection(mazeSetup, currentGrid[0], currentGrid[1], 3) && !visitedCoords[currentGrid[0] - 1][currentGrid[1]]) {
			enQueue(currentGrid[0] - 1, currentGrid[1]);
			grids_in_next_layer++;
		}

		// Go check next grid in current layer if there is more 
		if(grids_in_current_layer > 0) {
			grids_in_current_layer--;
		}	
	}

	return 0;
}

// Check whether the next grid is traversable
int checkDirection(grid_stats mazeSetup[MAZE_SIZE], int currentX, int currentY, int intDirection) {
	for (int mazeSize = 0; mazeSize < MAZE_SIZE; mazeSize++) {
		if (currentX == mazeSetup[mazeSize].coordinates[0] && currentY == mazeSetup[mazeSize].coordinates[1]) {
			if (mazeSetup[mazeSize].connections[intDirection] == 1) {
				return 1;
			}
		}
	}

	return 0;
}

// Queue to add x and y coordinates of a 2d Grid
void enQueue(int x, int y) {
	if (x < 0 || x > 4 || y < 0 || y > 3)
		printf("Invalid coords!\n");
	else if (rear == MAZE_SIZE - 1)
		printf("Queue is Full!\n");
	else {
		rear++;
		xCoords[rear] = x;
		yCoords[rear] = y;
		// printf("Inserted -> (%d, %d)\n", x, y);
	}
}

// Remove x and y coordinates from queue
void deQueue() {
	if (rear == -1)
		printf("Queue is Empty!!\n");
	else {
		// printf("Deleted : (%d, %d)\n", xCoords[0], yCoords[0]);
		for(int i = 0; i < rear; i++) {
			xCoords[i] = xCoords[i + 1];
			yCoords[i] = yCoords[i + 1];
		}
		rear--;
	}
}

// Check if the path to add is part of the correct path traversal
int isNextPath(grid_stats mazeSetup[MAZE_SIZE], int fullPath[2], int pathToAdd[2]) {

	/*
	First check if particular direction is traversable 0 = North, 1 = East, 2 = South, 3 = West
	Second check if the path to add is the same as the traversable path
	If yes return true, else return false
	*/
	if(checkDirection(mazeSetup, fullPath[0], fullPath[1], 0)) {
		if(fullPath[0] == pathToAdd[0] && (fullPath[1] - 1) == pathToAdd[1]) {
			return 1;
		}
	}

	if(checkDirection(mazeSetup, fullPath[0], fullPath[1], 1)) {
		if((fullPath[0] + 1) == pathToAdd[0] && fullPath[1] == pathToAdd[1]) {
			return 1;
		}
	}

	if(checkDirection(mazeSetup, fullPath[0], fullPath[1], 2)) {
		if(fullPath[0] == pathToAdd[0] && (fullPath[1] + 1) == pathToAdd[1]) {
			return 1;
		}
	}

	if(checkDirection(mazeSetup, fullPath[0], fullPath[1], 3)) {
		if((fullPath[0] - 1) == pathToAdd[0] && fullPath[1] == pathToAdd[1]) {
			return 1;
		}
	}
	return 0;
}

// Save the correct grid traversing path from an array of possible path movements 
void savePath(grid_stats mazeSetup[MAZE_SIZE], int numOfGrids, int actualPath[MAZE_SIZE][2], int possiblePaths[MAZE_SIZE][MAZE_SIZE][2], int endingGrid[2]) {
	for(int i = numOfGrids - 1; i >= 0; i--) {
		for (int j = 0; j < MAZE_SIZE; j++) {
			// If there are no more paths to check, end the current loop
			if (possiblePaths[i][j][0] == '\n')
				break;

			// Add the ending grid to actual path (should only succeed once), then break
			if(possiblePaths[i][j][0] == endingGrid[0] && possiblePaths[i][j][1] == endingGrid[1]) {
				actualPath[i][0] = possiblePaths[i][j][0];
				actualPath[i][1] = possiblePaths[i][j][1];
				break;
			}

			/*
			Check if the next possible path is part of the actual path
			If true, add into the variable actualPath, then break
			*/
			if (isNextPath(mazeSetup, actualPath[i + 1], possiblePaths[i][j])) {
				actualPath[i][0] = possiblePaths[i][j][0];
				actualPath[i][1] = possiblePaths[i][j][1];
				break;
			}
		}	
	}
}