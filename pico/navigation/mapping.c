#include "mappinggod.h"

void getTileInfo(mapping_struct *map, int *direction)
{
    // Stores FRONT, RIGHT, REAR, LEFT pings. Will contain 0-4, based on number of times wall is detected
    int get_walls_five[4] = {0};

    map->maze_info[map->mapped_count].coordinates[0] = map->current_position[0];
    map->maze_info[map->mapped_count].coordinates[1] = map->current_position[1];

    printf("visited: %d (%d,%d)\n", map->mapped_count, map->current_position[0], map->current_position[1]);
    printf("direction var: %d\n", *direction);

    printf("FRONT: %.2f\n", ULTRASONIC_getCM(ULTRASONIC_FRONT));
    printf("RIGHT: %.2f\n", ULTRASONIC_getCM(ULTRASONIC_RIGHT));
    printf("REAR: %.2f\n", ULTRASONIC_getCM(ULTRASONIC_REAR));
    printf("LEFT: %.2f\n", ULTRASONIC_getCM(ULTRASONIC_LEFT));

    // Ultrasonic (has to calculate with absolute direction, not relative direction)
    switch (*direction)
    {
    case 0:
        for (int i = 0; i < 5; i++)
        {
            get_walls_five[0] += ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[1] += ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[2] += ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[3] += ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)SAFETY_DISTANCE ? 1 : 0;
        }
        break;
    case 1:
        for (int i = 0; i < 5; i++)
        {
            get_walls_five[1] += ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[2] += ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[3] += ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[0] += ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)SAFETY_DISTANCE ? 1 : 0;
        }
        break;
    case 2:
        for (int i = 0; i < 5; i++)
        {
            get_walls_five[2] += ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[3] += ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[0] += ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[1] += ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)SAFETY_DISTANCE ? 1 : 0;
        }
        break;
    case 3:
        for (int i = 0; i < 5; i++)
        {
            get_walls_five[3] += ULTRASONIC_getCM(ULTRASONIC_FRONT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[0] += ULTRASONIC_getCM(ULTRASONIC_RIGHT) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[1] += ULTRASONIC_getCM(ULTRASONIC_REAR) > (float)SAFETY_DISTANCE ? 1 : 0;
            get_walls_five[2] += ULTRASONIC_getCM(ULTRASONIC_LEFT) > (float)SAFETY_DISTANCE ? 1 : 0;
        }
        break;
    }

    for (int i = 0; i < 4; i++)
    {
        map->maze_info[map->mapped_count].connections[i] = get_walls_five[i] >= 3 ? 1 : 0;
    }

    printf("%d (N) - %d (E) - %d (S) - %d(W)\n", get_walls_five[0], get_walls_five[1], get_walls_five[2], get_walls_five[3]);

    map->mapped_count++;
}

// chooses which direction to move in based on tile info
int chooseMovement(mapping_struct *map)
{
    // printf("%d %d %d %d\n", map->current_position[0], map->start_position[0], map->current_position[1], map->start_position[1]);
    // printf("%d %d %d %d\n", map->maze_info[0].connections[0], map->maze_info[0].connections[1], map->maze_info[2].connections[0], map->maze_info[0].connections[3]);
    // printf("visited: %d\n", map->visited_coords_size);

    // if visited is more than 0 AND car is at start position AND all 4 directions have been visited, exit
    // 4 direction check: !(0 || 0 || 0 || 0), all must either have no connection or be visited to fulfill 0, if all 0, !0 = 1
    if (map->visited_coords_size > 0 && (map->current_position[0] == map->start_position[0] && map->current_position[1] == map->start_position[1]) && !(
        (map->maze_info[0].connections[0] == 1 && !checkVisited(map, 0)) || (map->maze_info[0].connections[1] == 1 && !checkVisited(map, 1)) ||
        (map->maze_info[0].connections[2] == 1 && !checkVisited(map, 2)) || (map->maze_info[0].connections[3] == 1 && !checkVisited(map, 3))))
    {
        printf("im leaving!\n");
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
        chosen_direction = getBacktrackDirection(map);
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
                printf("found match at current: %d,%d checking: %d,%d\n", map->current_position[0], map->current_position[1],
                       map->visited_coords[i][0], map->visited_coords[i][1] - 1);
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
                printf("found match at current: %d,%d checking: %d,%d\n", map->current_position[0], map->current_position[1],
                       map->visited_coords[i][0] + 1, map->visited_coords[i][1]);
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
                printf("found match at current: %d,%d checking: %d,%d\n", map->current_position[0], map->current_position[1],
                       map->visited_coords[i][0], map->visited_coords[i][1] + 1);
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
                printf("found match at current: %d,%d checking: %d,%d\n", map->current_position[0], map->current_position[1],
                       map->visited_coords[i][0] - 1, map->visited_coords[i][1]);
                flag = 1;
                break;
            }
        }
        break;
    }

    return flag;
}

int getBacktrackDirection(mapping_struct *map)
{
    int x_diff = 0;

    x_diff = map->current_position[0] - map->bot_path[(map->bot_path_size - 1) - 1][0];
    if (x_diff != 0)
    {
        return x_diff > 0 ? 3 : 1;
    }

    int y_diff = 0;

    y_diff = map->current_position[1] - map->bot_path[(map->bot_path_size - 1) - 1][1];
    if (y_diff != 0)
    {
        return y_diff > 0 ? 0 : 2;
    }
}

// returns a sorted array of structures. also sets an int pointer's values to the bots 0,0 position.
void *sortMappedMaze(mapping_struct *map)
{
    grid_stats sorted_maze[MAZE_SIZE];
    int smallest_x = 0, smallest_y = 0, largest_x = 0, largest_y = 0;
    int counter = 0;
    printf("begin sort!\n");
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        if (map->maze_info[i].coordinates[0] < smallest_x)
            smallest_x = map->maze_info[i].coordinates[0];
        if (map->maze_info[i].coordinates[1] < smallest_y)
            smallest_y = map->maze_info[i].coordinates[1];
        if (map->maze_info[i].coordinates[0] > largest_x)
            largest_x = map->maze_info[i].coordinates[0];
        if (map->maze_info[i].coordinates[1] > largest_y)
            largest_y = map->maze_info[i].coordinates[1];
    }

    for (int y = smallest_y; y <= largest_y; y++)
    {
        for (int x = smallest_x; x <= largest_x; x++)
        {
            for (int i = 0; i < MAZE_SIZE; i++)
            {
                if (map->maze_info[i].coordinates[0] == x && map->maze_info[i].coordinates[1] == y)
                {
                    sorted_maze[counter].coordinates[0] = map->maze_info[i].coordinates[0];
                    sorted_maze[counter].coordinates[1] = map->maze_info[i].coordinates[1];
                    sorted_maze[counter].connections[0] = map->maze_info[i].connections[0];
                    sorted_maze[counter].connections[1] = map->maze_info[i].connections[1];
                    sorted_maze[counter].connections[2] = map->maze_info[i].connections[2];
                    sorted_maze[counter].connections[3] = map->maze_info[i].connections[3];
                    counter++;
                    break;
                }
            }
        }
    }
    printf("sorted maze filled!\n");
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        sorted_maze[i].coordinates[0] += -smallest_x;
        sorted_maze[i].coordinates[1] += -smallest_y;
    }
     printf("sorted maze normalised!\n");

    int bot_pos_x = -smallest_x;
    int bot_pos_y = -smallest_y;
    printf("bot position: (%d %d)\n", bot_pos_x, bot_pos_y);
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        printf("(%d,%d) %d - %d - %d - %d\n", sorted_maze[i].coordinates[0], sorted_maze[i].coordinates[1],
               sorted_maze[i].connections[0], sorted_maze[i].connections[1], sorted_maze[i].connections[2], sorted_maze[i].connections[3]);
    }
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