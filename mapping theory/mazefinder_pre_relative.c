#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 16
#define MAZE_SIZE 20

typedef struct {
	int coordinates[2];
	int connections[4];	// N, E, S, W
}grid_stats;

void populateProvidedMaze(grid_stats*);
int mapMaze(grid_stats*, grid_stats*);
int checkVisited(int, int, int**, int);

int main() {
	grid_stats* provided_maze = (grid_stats*)malloc(MAZE_SIZE * sizeof(grid_stats));
	populateProvidedMaze(provided_maze);

	grid_stats* mapped_maze = (grid_stats*)malloc(MAZE_SIZE * sizeof(grid_stats));
	mapMaze(provided_maze, mapped_maze);

	for (int i = 0; i < MAZE_SIZE; i++) {
		printf("(%d, %d): %d, %d, %d, %d\n", mapped_maze[i].coordinates[0], mapped_maze[i].coordinates[1], mapped_maze[i].connections[0],
			mapped_maze[i].connections[1], mapped_maze[i].connections[2], mapped_maze[i].connections[3]);
	}

	return 0;
}

void populateProvidedMaze(grid_stats* provided_maze) {
	FILE* open_file;
	char buffer[BUFFER_SIZE];
	char* token;
	int counter = 0, phase = 0;


	open_file = fopen("conversion.txt", "r"); // Opens filename

	while (fgets(buffer, BUFFER_SIZE, open_file))
	{
		switch (phase) {
		case 0:
			token = strtok(buffer, ",");
			provided_maze[counter].coordinates[0] = strtol(token, NULL, 10);
			token = strtok(NULL, ",");
			provided_maze[counter].coordinates[1] = strtol(token, NULL, 10);
			phase++;
			break;
		case 1:
			token = strtok(buffer, ",");
			provided_maze[counter].connections[0] = strtol(token, NULL, 10);
			token = strtok(NULL, ",");
			provided_maze[counter].connections[1] = strtol(token, NULL, 10);
			token = strtok(NULL, ",");
			provided_maze[counter].connections[2] = strtol(token, NULL, 10);
			token = strtok(NULL, ",");
			provided_maze[counter].connections[3] = strtol(token, NULL, 10);
			counter++;
			phase--;
			break;
		default:
			break;
		}
	}
}

int mapMaze(grid_stats* provided_maze, grid_stats* mapped_maze) {
	int mapped_count = 0;
	int** visited_coords = (int**)malloc(MAZE_SIZE * sizeof(int*));
	for (int i = 0; i < MAZE_SIZE; i++)
		visited_coords[i] = (int*)malloc(2 * sizeof(int));
	int visited_coords_size = 0;
	int** bot_path = (int**)malloc(MAZE_SIZE * sizeof(int*));
	for (int i = 0; i < MAZE_SIZE; i++)
		bot_path[i] = (int*)malloc(2 * sizeof(int));
	int bot_path_size = 0;
	int start_position[2] = { 1,1 };
	int current_position[2] = { start_position[0], start_position[1] };	// position = {y-axis, x-axis}

	// adding start coords to visited and bot_path
	visited_coords[visited_coords_size][0] = start_position[0];
	visited_coords[visited_coords_size][1] = start_position[1];
	visited_coords_size++;
	bot_path[bot_path_size][0] = start_position[0];
	bot_path[bot_path_size][1] = start_position[1];
	bot_path_size++;

	printf("visited: %d (%d,%d)\n", visited_coords_size, current_position[0], current_position[1]);

	// looks for matching coord info in provided_maze, add to mapped_maze
	for (int i = 0; i < MAZE_SIZE; i++) {
		if (current_position[0] == provided_maze[i].coordinates[0] && current_position[1] == provided_maze[i].coordinates[1]) {
			mapped_maze[mapped_count].coordinates[0] = provided_maze[i].coordinates[0];
			mapped_maze[mapped_count].coordinates[1] = provided_maze[i].coordinates[1];
			mapped_maze[mapped_count].connections[0] = provided_maze[i].connections[0];
			mapped_maze[mapped_count].connections[1] = provided_maze[i].connections[1];
			mapped_maze[mapped_count].connections[2] = provided_maze[i].connections[2];
			mapped_maze[mapped_count].connections[3] = provided_maze[i].connections[3];
			mapped_count++;
			break;
		}
	}

	// make the first move
	if (mapped_maze[0].connections[0] == 1) {
		current_position[0] = current_position[0] - 1;
		// no movement on x-axis
	}
	else if (mapped_maze[0].connections[1] == 1) {
		current_position[1] = current_position[1] + 1;
		// no movement on y-axis
	}
	else if (mapped_maze[0].connections[2] == 1) {
		current_position[0] = current_position[0] + 1;
		// no movement on x-axis
	}
	else if (mapped_maze[0].connections[3] == 1) {
		current_position[1] = current_position[1] - 1;
		// no movement on y-axis
	}

	// add coords to visited
	visited_coords[visited_coords_size][0] = current_position[0];
	visited_coords[visited_coords_size][1] = current_position[1];
	visited_coords_size++;

	printf("visited: %d (%d,%d)\n", visited_coords_size, current_position[0], current_position[1]);

	int backStepped = 0;

	// exit condition: keep moving until reached start position, and all directions in start position are visited
	// when reached start: cycles through each direction, if a direction is determined to have no match, continue, else end loop
	while (!(current_position[0] == start_position[0] && current_position[1] == start_position[1]) ||
		(mapped_maze[0].connections[0] == 1 && !checkVisited(start_position[0] - 1, start_position[1], visited_coords, visited_coords_size)) ||
		(mapped_maze[0].connections[1] == 1 && !checkVisited(start_position[0], start_position[1] + 1, visited_coords, visited_coords_size)) ||
		(mapped_maze[0].connections[2] == 1 && !checkVisited(start_position[0] + 1, start_position[1], visited_coords, visited_coords_size)) ||
		(mapped_maze[0].connections[3] == 1 && !checkVisited(start_position[0], start_position[1] - 1, visited_coords, visited_coords_size))
		) {

		// add current position to mapped_maze and bot_path sets, only if the previous iteration was not a backstep
		// if it was, mapped_maze already contains that coord, bot_path was already updated
		if (!backStepped) {
			bot_path[bot_path_size][0] = current_position[0];
			bot_path[bot_path_size][1] = current_position[1];
			bot_path_size++;

			// looks for matching coord info in provided_maze, add to mapped_maze
			for (int i = 0; i < MAZE_SIZE; i++) {
				if (current_position[0] == provided_maze[i].coordinates[0] && current_position[1] == provided_maze[i].coordinates[1]) {
					mapped_maze[mapped_count].coordinates[0] = provided_maze[i].coordinates[0];
					mapped_maze[mapped_count].coordinates[1] = provided_maze[i].coordinates[1];
					mapped_maze[mapped_count].connections[0] = provided_maze[i].connections[0];
					mapped_maze[mapped_count].connections[1] = provided_maze[i].connections[1];
					mapped_maze[mapped_count].connections[2] = provided_maze[i].connections[2];
					mapped_maze[mapped_count].connections[3] = provided_maze[i].connections[3];
					mapped_count++;
					break;
				}
			}
		}
		backStepped = 0;

		// locate current position in mapped_maze to use stored connections
		int currentPositionCorrespondingMapIndex = 0;
		for (int i = 0; i < mapped_count; i++) {
			if (mapped_maze[i].coordinates[0] == current_position[0] && mapped_maze[i].coordinates[1] == current_position[1]) {
				currentPositionCorrespondingMapIndex = i;
				break;
			}
		}

		// check north, east, south, west for opening, if opening exists check if resultant coord is in visited array
		// no unvisited coord found, backstep
		if (mapped_maze[currentPositionCorrespondingMapIndex].connections[0] == 1 && !checkVisited(current_position[0] - 1, current_position[1], visited_coords, visited_coords_size)) {
			current_position[0] = current_position[0] - 1;
			// no movement on x-axis

			visited_coords[visited_coords_size][0] = current_position[0];
			visited_coords[visited_coords_size][1] = current_position[1];
			visited_coords_size++;
		}
		else if (mapped_maze[currentPositionCorrespondingMapIndex].connections[1] == 1 && !checkVisited(current_position[0], current_position[1] + 1, visited_coords, visited_coords_size)) {
			current_position[1] = current_position[1] + 1;
			// no movement on y-axis

			visited_coords[visited_coords_size][0] = current_position[0];
			visited_coords[visited_coords_size][1] = current_position[1];
			visited_coords_size++;
		}
		else if (mapped_maze[currentPositionCorrespondingMapIndex].connections[2] == 1 && !checkVisited(current_position[0] + 1, current_position[1], visited_coords, visited_coords_size)) {
			current_position[0] = current_position[0] + 1;
			// no movement on x-axis

			visited_coords[visited_coords_size][0] = current_position[0];
			visited_coords[visited_coords_size][1] = current_position[1];
			visited_coords_size++;
		}
		else if (mapped_maze[currentPositionCorrespondingMapIndex].connections[3] == 1 && !checkVisited(current_position[0], current_position[1] - 1, visited_coords, visited_coords_size)) {
			current_position[1] = current_position[1] - 1;
			// no movement on y-axis

			visited_coords[visited_coords_size][0] = current_position[0];
			visited_coords[visited_coords_size][1] = current_position[1];
			visited_coords_size++;
		}
		else {
			// step back once
			bot_path_size--;
			current_position[0] = bot_path[bot_path_size][0];
			current_position[1] = bot_path[bot_path_size][1];

			// indicate most recent operation is backstep
			backStepped = 1;
		}

		printf("visited: %d (%d,%d) <backstepped: %d>\n", visited_coords_size, current_position[0], current_position[1], backStepped);
	}
	return mapped_count;
}

int checkVisited(int position_0, int position_1, int** visited_coords, int visited_coords_size) {
	int flag = 0;

	for (int i = 0; i < visited_coords_size; i++) {
		if (visited_coords[i][0] == position_0 && visited_coords[i][1] == position_1) {
			flag = 1;
			break;
		}
	}
	return flag;
}