#include "mappinggod.h"

void getTileInfo(mapping_struct *map, int *direction)
{
    map->maze_info[map->mapped_count].coordinates[0] = map->current_position[0];
    map->maze_info[map->mapped_count].coordinates[1] = map->current_position[1];

    printf("visited: %d (%d,%d)\n", map->mapped_count, map->current_position[0], map->current_position[1]);
    printf("direction var: %d\n", *direction);

    // Ultrasonic (has to calculate with absolute direction, not relative direction)
    switch (*direction)
    {
    case 0:
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)10 ? 1 : 0;
        break;
    case 1:
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)10 ? 1 : 0;
        break;
    case 2:
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)10 ? 1 : 0;
        break;
    case 3:
        map->maze_info[map->mapped_count].connections[3] = ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[0] = ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[1] = ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)10 ? 1 : 0;
        map->maze_info[map->mapped_count].connections[2] = ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)10 ? 1 : 0;
        break;
    }

    // printf("%.2f\n", ULTRASONIC_getCM(ULTRASONIC_FRONT));
    // printf("%.2f\n", ULTRASONIC_getCM(ULTRASONIC_RIGHT));
    // printf("%.2f\n", ULTRASONIC_getCM(ULTRASONIC_REAR));
    // printf("%.2f\n", ULTRASONIC_getCM(ULTRASONIC_LEFT));
    printf("NESW: %d - %d - %d - %d)\n", map->maze_info[map->mapped_count].connections[0], map->maze_info[map->mapped_count].connections[1],
           map->maze_info[map->mapped_count].connections[2], map->maze_info[map->mapped_count].connections[3]);

    map->mapped_count++;
}

// chooses which direction to move in based on tile info
int chooseMovement(mapping_struct *map)
{
    // printf("%d %d %d %d\n", map->current_position[0], map->start_position[0], map->current_position[1], map->start_position[1]);
    // printf("%d %d %d %d\n", map->maze_info[0].connections[0], map->maze_info[0].connections[1], map->maze_info[2].connections[0], map->maze_info[0].connections[3]);
    // printf("visited: %d\n", map->visited_coords_size);

    // if visited is more than 0 AND car is at start position AND all 4 directions have been visited, exit
    if (map->visited_coords_size > 0 && (map->current_position[0] == map->start_position[0] && map->current_position[1] == map->start_position[1]) &&
        (map->maze_info[0].connections[0] == 1 && checkVisited(map, 0)) &&
        (map->maze_info[0].connections[1] == 1 && checkVisited(map, 1)) &&
        (map->maze_info[0].connections[2] == 1 && checkVisited(map, 2)) &&
        (map->maze_info[0].connections[3] == 1 && checkVisited(map, 3)))
    {
        return -1;
    }

    if (!(map->backstepped))
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
    // mapped_count - 1 must be checked
    if (checkOpening(map, 0) && !checkVisited(map, 0))
    {
        // no movement on x-axis
        map->current_position[1] = map->current_position[1] - 1;
        chosen_direction = 0;
    }
    else if (checkOpening(map, 1) && !checkVisited(map, 1))
    {
        map->current_position[0] = map->current_position[0] + 1;
        // no movement on y-axis
        chosen_direction = 1;
    }
    else if (checkOpening(map, 2) && !checkVisited(map, 2))
    {
        // no movement on x-axis
        map->current_position[1] = map->current_position[1] + 1;
        chosen_direction = 2;
    }
    else if (checkOpening(map, 3) && !checkVisited(map, 3))
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

int checkOpening(mapping_struct *map, int direction)
{
    for (int i = 0; i < map->mapped_count; i++)
    {
        if (map->maze_info[i].coordinates[0] == map->current_position[0] && map->maze_info[i].coordinates[1] == map->current_position[1])
        {
            return map->maze_info[i].connections[direction];
        }
    }
    return 0;
}

// helper function to check if specified coords are in visited array.
int checkVisited(mapping_struct *map, int direction)
{
    int flag = 0;

    switch (direction)
    {
    case 0:
        for (int i = 0; i < map->visited_coords_size; i++)
        {
            if (map->visited_coords[i][0] == map->current_position[0] && map->visited_coords[i][1] == map->current_position[1] - 1)
            {
                flag = 1;
                break;
            }
        }
        break;
    case 1:
        for (int i = 0; i < map->visited_coords_size; i++)
        {
            if (map->visited_coords[i][0] == map->current_position[0] + 1 && map->visited_coords[i][1] == map->current_position[1])
            {
                flag = 1;
                break;
            }
        }
        break;
    case 2:
        for (int i = 0; i < map->visited_coords_size; i++)
        {
            if (map->visited_coords[i][0] == map->current_position[0] && map->visited_coords[i][1] == map->current_position[1] + 1)
            {
                flag = 1;
                break;
            }
        }
        break;
    case 3:
        for (int i = 0; i < map->visited_coords_size; i++)
        {
            if (map->visited_coords[i][0] == map->current_position[0] - 1 && map->visited_coords[i][1] == map->current_position[1])
            {
                flag = 1;
                break;
            }
        }
        break;
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