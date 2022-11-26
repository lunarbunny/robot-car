#include "mappinggod.h"

int xCoords[MAZE_SIZE], yCoords[MAZE_SIZE], rear = -1; // For navigation

// Queue to add x and y coordinates of a 2d Grid
void enQueue(int x, int y)
{
	if (x < 0 || x > 4 || y < 0 || y > 3)
		printf("Invalid coords!\n");
	else if (rear == MAZE_SIZE - 1)
		printf("Queue is Full!\n");
	else
	{
		rear++;
		xCoords[rear] = x;
		yCoords[rear] = y;
		// printf("Inserted -> (%d, %d)\n", x, y);
	}
}

// Remove x and y coordinates from queue
void deQueue()
{
	if (rear == -1)
		printf("Queue is Empty!!\n");
	else
	{
		// printf("Deleted : (%d, %d)\n", xCoords[0], yCoords[0]);
		for (int i = 0; i < rear; i++)
		{
			xCoords[i] = xCoords[i + 1];
			yCoords[i] = yCoords[i + 1];
		}
		rear--;
	}
}

void computeShortestPath(mapping_struct map, compute_path *path, int *state)
{
	findPath(map, path);
	savePath(map, path);
	*state = STATE_EXECUTING_PATH;
}

// I guess its a BFS check to find path from starting grid to ending grid
void findPath(mapping_struct map, compute_path *path)
{

	int col = map.maze_info[MAZE_SIZE - 1].coordinates[0] + 1;
	int row = map.maze_info[MAZE_SIZE - 1].coordinates[1] + 1;
	int **visitedCoords = calloc(col, sizeof(int *)); // Initialise visited grids as 0
	mallocChecker(visitedCoords);
	for (int i = 0; i < col; i++)
	{
		visitedCoords[i] = calloc(row, sizeof(int));
		mallocChecker(visitedCoords[i]);
	}

	int grids_in_next_layer = 0;	// The number of grids in the next layer of nodes to check
	int grids_in_current_layer = 0; // The current layer's number of nodes to check
	path->move_count = 0;			// Number of grids to traverse
	int current_layer = 0;			// Current layer of nodes for possible paths

	enQueue(path->current_grid[0], path->current_grid[1]); // Queue the starting coords
	// printf("X: %d, Y: %d\n", xCoords[0], yCoords[0]);

	while (rear >= 0)
	{
		path->current_grid[0] = xCoords[0];								 // Current X
		path->current_grid[1] = yCoords[0];								 // Current Y
		visitedCoords[path->current_grid[0]][path->current_grid[1]] = 1; // Mark the current grid as visited
		// printf("%d, %d, %d\n", xCoords[0], yCoords[0], visitedCoords[path->current_grid[0]][path->current_grid[1]]);

		// Check if there are no more grids in current layer
		if (grids_in_current_layer == 0)
		{
			grids_in_current_layer = grids_in_next_layer;
			grids_in_next_layer = 0;

			if (path->move_count != 0)
			{
				path->possible_paths[path->move_count - 1][current_layer][0] = '\n';
				path->possible_paths[path->move_count - 1][current_layer][1] = '\n';
			}

			path->move_count++;
			current_layer = 0;
		}

		path->possible_paths[path->move_count - 1][current_layer][0] = xCoords[0];
		path->possible_paths[path->move_count - 1][current_layer][1] = yCoords[0];
		current_layer++;
		deQueue(); // Remove current grid from the queue

		// Check if current grid is destination
		if (path->current_grid[0] == path->ending_grid[0] && path->current_grid[1] == path->ending_grid[1])
		{
			path->possible_paths[path->move_count - 1][current_layer][0] = '\n';
			path->possible_paths[path->move_count - 1][current_layer][1] = '\n';
			return;
		}

		// Check all possible grids that you can traverse to from current grid
		if (checkDirection(map, path->current_grid, 0) && !visitedCoords[path->current_grid[0]][path->current_grid[1] - 1])
		{
			enQueue(path->current_grid[0], path->current_grid[1] - 1);
			grids_in_next_layer++;
		}
		if (checkDirection(map, path->current_grid, 1) && !visitedCoords[path->current_grid[0] + 1][path->current_grid[1]])
		{
			enQueue(path->current_grid[0] + 1, path->current_grid[1]);
			grids_in_next_layer++;
		}
		if (checkDirection(map, path->current_grid, 2) && !visitedCoords[path->current_grid[0]][path->current_grid[1] + 1])
		{
			enQueue(path->current_grid[0], path->current_grid[1] + 1);
			grids_in_next_layer++;
		}
		if (checkDirection(map, path->current_grid, 3) && !visitedCoords[path->current_grid[0] - 1][path->current_grid[1]])
		{
			enQueue(path->current_grid[0] - 1, path->current_grid[1]);
			grids_in_next_layer++;
		}

		// Go check next grid in current layer if there is more
		if (grids_in_current_layer > 0)
		{
			grids_in_current_layer--;
		}
	}
}

// Check whether the next grid is traversable
int checkDirection(mapping_struct map, int current_grid[2], int intDirection)
{
	for (int mazeSize = 0; mazeSize < MAZE_SIZE; mazeSize++)
	{
		if (current_grid[0] == map.maze_info[mazeSize].coordinates[0] && current_grid[1] == map.maze_info[mazeSize].coordinates[1])
		{
			if (map.maze_info[mazeSize].connections[intDirection] == 1)
			{
				return 1;
			}
		}
	}

	return 0;
}

// Check if the path to add is part of the correct path traversal
int isNextPath(mapping_struct map, int fullPath[2], int pathToAdd[2])
{
	/*
	First check if particular direction is traversable 0 = North, 1 = East, 2 = South, 3 = West
	Second check if the path to add is the same as the traversable path
	If yes return true, else return false
	*/
	if (checkDirection(map, fullPath, 0))
	{
		if (fullPath[0] == pathToAdd[0] && (fullPath[1] - 1) == pathToAdd[1])
		{
			return 1;
		}
	}

	if (checkDirection(map, fullPath, 1))
	{
		if ((fullPath[0] + 1) == pathToAdd[0] && fullPath[1] == pathToAdd[1])
		{
			return 1;
		}
	}

	if (checkDirection(map, fullPath, 2))
	{
		if (fullPath[0] == pathToAdd[0] && (fullPath[1] + 1) == pathToAdd[1])
		{
			return 1;
		}
	}

	if (checkDirection(map, fullPath, 3))
	{
		if ((fullPath[0] - 1) == pathToAdd[0] && fullPath[1] == pathToAdd[1])
		{
			return 1;
		}
	}
	return 0;
}

// Save the correct grid traversing path from an array of possible path movements
void savePath(mapping_struct map, compute_path *path)
{
	for (int i = path->move_count - 1; i >= 0; i--)
	{
		for (int j = 0; j < MAZE_SIZE; j++)
		{
			// If there are no more paths to check, end the current loop
			if (path->possible_paths[i][j][0] == '\n')
				break;

			// Add the ending grid to actual path (should only succeed once), then break
			if (path->possible_paths[i][j][0] == path->ending_grid[0] && path->possible_paths[i][j][1] == path->ending_grid[1])
			{
				path->actual_path[i][0] = path->possible_paths[i][j][0];
				path->actual_path[i][1] = path->possible_paths[i][j][1];
				break;
			}

			/*
			Check if the next possible path is part of the actual path
			If true, add into the variable actualPath, then break
			*/
			if (isNextPath(map, path->actual_path[i + 1], path->possible_paths[i][j]))
			{
				path->actual_path[i][0] = path->possible_paths[i][j][0];
				path->actual_path[i][1] = path->possible_paths[i][j][1];
				break;
			}
		}
	}
}