#include "mappinggod.h"

void getTileInfo(mapping_struct *map, int *direction)
{
    map->maze_info[map->mapped_count].coordinates[0] = map->current_position[0];
    map->maze_info[map->mapped_count].coordinates[1] = map->current_position[1];

    // Ultrasonic (has to calculate with absolute direction, not relative direction)
    switch (*direction)
    {
    case 0:
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (uint64_t)10 ? 1 : 0;
        break;
    case 1:
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (uint64_t)10 ? 1 : 0;
        break;
    case 2:
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (uint64_t)10 ? 1 : 0;
        break;
    case 3:
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (uint64_t)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (uint64_t)10 ? 1 : 0;
        break;
    }

    printf("%u\n", (long) ULTRASONIC_getCM(ULTRASONIC_FRONT));
    printf("%u\n", (long) ULTRASONIC_getCM(ULTRASONIC_RIGHT));
    printf("%u\n", (long) ULTRASONIC_getCM(ULTRASONIC_REAR));
    printf("%u\n", (long) ULTRASONIC_getCM(ULTRASONIC_LEFT));
    printf("visited: %d (%d,%d)\n", map->mapped_count, map->current_position[0], map->current_position[1]);
    printf("NESW: %d - %d - %d - %d)\n", map->maze_info[map->mapped_count].connections[0], map->maze_info[map->mapped_count].connections[1],
           map->maze_info[map->mapped_count].connections[2], map->maze_info[map->mapped_count].connections[3]);

    (map->mapped_count)++;
}

// maps the maze based on provided map. returns number of tiles visited.
int chooseMovement(mapping_struct *map)
{
    if (map->current_position[0] == map->start_position[0] && map->current_position[1] == map->start_position[1] ||
        (map->maze_info[0].connections[0] == 1 && !checkVisited(map->start_position[0], map->start_position[1] - 1, map->visited_coords, map->visited_coords_size)) ||
        (map->maze_info[0].connections[1] == 1 && !checkVisited(map->start_position[0] + 1, map->start_position[1] - 1, map->visited_coords, map->visited_coords_size)) ||
        (map->maze_info[0].connections[2] == 1 && !checkVisited(map->start_position[0], map->start_position[1] - 1 + 1, map->visited_coords, map->visited_coords_size)) ||
        (map->maze_info[0].connections[3] == 1 && !checkVisited(map->start_position[0] - 1, map->start_position[1] - 1, map->visited_coords, map->visited_coords_size)))
    {
        return -1;
    }

    if (!map->backstepped)
    {
        // adding coords to visited and bot_path
        map->visited_coords[map->visited_coords_size][0] = map->current_position[0];
        map->visited_coords[map->visited_coords_size][1] = map->current_position[1];
        (map->visited_coords_size)++;
        map->bot_path[map->bot_path_size][0] = map->current_position[0];
        map->bot_path[map->bot_path_size][1] = map->current_position[1];
        (map->bot_path_size)++;
    }

    map->backstepped = 0;

    int chosen_direction = 0;

    // check north, east, south, west for opening, if opening exists check if resultant coord is in visited array.
    // once opening chosen, update current position, but dont actually scan anything, updates will be done in next cycle
    // no unvisited coord found, backstep
    if (map->maze_info[map->mapped_count].connections[0] == 1 &&
        !checkVisited(map->current_position[0], map->current_position[1] - 1, map->visited_coords, map->visited_coords_size))
    {
        // no movement on x-axis
        map->current_position[1] = map->current_position[1] - 1;
        chosen_direction = 0;
    }
    else if (map->maze_info[map->mapped_count].connections[1] == 1 &&
             !checkVisited(map->current_position[0] + 1, map->current_position[1], map->visited_coords, map->visited_coords_size))
    {
        map->current_position[0] = map->current_position[0] + 1;
        // no movement on y-axis
        chosen_direction = 1;
    }
    else if (map->maze_info[map->mapped_count].connections[2] == 1 &&
             !checkVisited(map->current_position[0], map->current_position[1] + 1, map->visited_coords, map->visited_coords_size))
    {
        // no movement on x-axis
        map->current_position[1] = map->current_position[1] + 1;
        chosen_direction = 2;
    }
    else if (map->maze_info[map->mapped_count].connections[3] == 1 &&
             !checkVisited(map->current_position[0] - 1, map->current_position[1], map->visited_coords, map->visited_coords_size))
    {
        map->current_position[0] = map->current_position[0] - 1;
        // no movement on y-axis
        chosen_direction = 3;
    }
    else
    {
        // step back once
        (map->bot_path_size)--;
        map->current_position[0] = map->bot_path[map->bot_path_size - 1][0];
        map->current_position[1] = map->bot_path[map->bot_path_size - 1][1];

        // indicate most recent operation is backstep
        map->backstepped = 1;
    }

    printf("new target: %d (%d,%d)\n", chosen_direction, map->current_position[0], map->current_position[1]);

    return chosen_direction;
}

// helper function to check if specified coords are in visited array.
int checkVisited(int position_0, int position_1, int **visited_coords, int visited_coords_size)
{
    int flag = 0;

    for (int i = 0; i < visited_coords_size; i++)
    {
        if (visited_coords[i][0] == position_0 && visited_coords[i][1] == position_1)
        {
            flag = 1;
            break;
        }
    }
    return flag;
}

// returns a sorted array of structures. also sets an int pointer's values to the bots 0,0 position.
grid_stats *sortMappedMaze(grid_stats *mapped_maze, int *position)
{
    grid_stats *sorted_maze = (grid_stats *)malloc(MAZE_SIZE * sizeof(grid_stats));
    mallocChecker(sorted_maze);
    int smallest_x = 0, smallest_y = 0, largest_x = 0, largest_y = 0;
    int counter = 0;

    for (int i = 0; i < MAZE_SIZE; i++)
    {
        if (mapped_maze[i].coordinates[0] < smallest_x)
            smallest_x = mapped_maze[i].coordinates[0];
        if (mapped_maze[i].coordinates[1] < smallest_y)
            smallest_y = mapped_maze[i].coordinates[1];
        if (mapped_maze[i].coordinates[0] > largest_x)
            largest_x = mapped_maze[i].coordinates[0];
        if (mapped_maze[i].coordinates[1] > largest_y)
            largest_y = mapped_maze[i].coordinates[1];
    }

    for (int y = smallest_y; y <= largest_y; y++)
    {
        for (int x = smallest_x; x <= largest_x; x++)
        {
            for (int i = 0; i < MAZE_SIZE; i++)
            {
                if (mapped_maze[i].coordinates[0] == x && mapped_maze[i].coordinates[1] == y)
                {
                    sorted_maze[counter].coordinates[0] = mapped_maze[i].coordinates[0];
                    sorted_maze[counter].coordinates[1] = mapped_maze[i].coordinates[1];
                    sorted_maze[counter].connections[0] = mapped_maze[i].connections[0];
                    sorted_maze[counter].connections[1] = mapped_maze[i].connections[1];
                    sorted_maze[counter].connections[2] = mapped_maze[i].connections[2];
                    sorted_maze[counter].connections[3] = mapped_maze[i].connections[3];
                    counter++;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < MAZE_SIZE; i++)
    {
        sorted_maze[i].coordinates[0] += -smallest_x;
        sorted_maze[i].coordinates[1] += -smallest_y;
    }

    position[0] = -smallest_x;
    position[1] = -smallest_y;

    return sorted_maze;
}

// ensure malloc success, eliminate catastrophic failure.
void mallocChecker(void *ptr)
{
    if (ptr == NULL)
    {
        printf("No memory!\n");
        exit(1);
    }
}