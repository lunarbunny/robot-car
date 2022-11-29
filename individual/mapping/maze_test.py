from pyamaze import maze

storing_array = []
test_cases = 100
x_length = 5
y_length = 4

for i in range(test_cases):
    m = maze(y_length, x_length)
    m.CreateMaze(loopPercent=100)
    storing_array.append(m.maze_map)

with open('all.txt', 'w') as f:
    for i in range(test_cases):
        f.write(f"\"{i}\": {str(storing_array[i])}\n")


class grid_struct:
    def __init__(self, coord, openings):
        self.coord = coord
        self.openings = openings

# maze_list = {(1, 1): {'E': 1, 'W': 0, 'N': 0, 'S': 1}, (2, 1): {'E': 1, 'W': 0, 'N': 1, 'S': 0}, (3, 1): {'E': 1, 'W': 0, 'N': 0, 'S': 1}, (4, 1): {'E': 1, 'W': 0, 'N': 1, 'S': 0}, (1, 2): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (2, 2): {'E': 0, 'W': 1, 'N': 0, 'S': 1}, (3, 2): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (4, 2): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (1, 3): {'E': 1, 'W': 1, 'N': 0, 'S': 1}, (2, 3): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (3, 3): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (4, 3): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (1, 4): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (2, 4): {'E': 1, 'W': 0, 'N': 0, 'S': 1}, (3, 4): {'E':
#                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  0, 'W': 1, 'N': 1, 'S': 1}, (4, 4): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (1, 5): {'E': 0, 'W': 1, 'N': 0, 'S': 1}, (2, 5): {'E': 0, 'W': 1, 'N': 1, 'S': 1}, (3, 5): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 5): {'E': 0, 'W': 1, 'N': 1, 'S': 0}}


def map_maze(maze, start_pos):
    bot_path = []           # path the bot will store
    visited = []            # visited coords list
    debug_full_path = []    # for demo only, the full path including backtracking
    map_info = []           # stores all grid info, coords and openings
    current_pos = start_pos

    # the bot's starting position specs
    visited.append(current_pos)
    bot_path.append(current_pos)
    debug_full_path.append(current_pos)
    map_info = [grid_struct(current_pos, maze[current_pos])]

    # the bot will attempt to navigate with preference for north, followed by a clockwise check
    if maze[current_pos].get('N') == 1:
        current_pos = (current_pos[0]-1, current_pos[1])
    elif maze[current_pos].get('E') == 1:
        current_pos = (current_pos[0], current_pos[1]+1)
    elif maze[current_pos].get('S') == 1:
        current_pos = (current_pos[0]+1, current_pos[1])
    elif maze[current_pos].get('W') == 1:
        current_pos = (current_pos[0], current_pos[1]-1)

    backstepped = False  # variable to store if there's no way to go, so the bot must backstep

    # keep moving until reached original position len(map_info) != 20
    while (current_pos != start_pos or
    (maze[start_pos].get('N') == 1 and (start_pos[0]-1, start_pos[1]) not in visited) or 
    (maze[start_pos].get('E') == 1 and (start_pos[0], start_pos[1]+1) not in visited) or 
    (maze[start_pos].get('S') == 1 and (start_pos[0]+1, start_pos[1]) not in visited) or 
    (maze[start_pos].get('W') == 1 and (start_pos[0], start_pos[1]-1) not in visited)):
        # only add to bot_path if the resultant move is not a backtrack
        if not backstepped:
            visited.append(current_pos)
            bot_path.append(current_pos)
            map_info.append(grid_struct(
                current_pos, maze[current_pos]))   # store info

        backstepped = False
        debug_full_path.append(current_pos)

        # the bot will attempt to navigate with preference for north, followed by a clockwise check
        # if no eligible paths, remove the current position from the bot's path, then move back to the previous position
        if maze[current_pos].get('N') == 1 and (current_pos[0]-1, current_pos[1]) not in visited:
            current_pos = (current_pos[0]-1, current_pos[1])
            visited.append(current_pos)
        elif maze[current_pos].get('E') == 1 and (current_pos[0], current_pos[1]+1) not in visited:
            current_pos = (current_pos[0], current_pos[1]+1)
            visited.append(current_pos)
        elif maze[current_pos].get('S') == 1 and (current_pos[0]+1, current_pos[1]) not in visited:
            current_pos = (current_pos[0]+1, current_pos[1])
            visited.append(current_pos)
        elif maze[current_pos].get('W') == 1 and (current_pos[0], current_pos[1]-1) not in visited:
            current_pos = (current_pos[0], current_pos[1]-1)
            visited.append(current_pos)
        else:
            bot_path.pop()
            current_pos = bot_path[-1]
            backstepped = True

    return len(map_info)
    # print("map tiles stored: ", len(map_info))
    # print("visit order: ", debug_full_path)
    # print("storing order: ", visited)

    # for i in range(len(map_info)):
    #     print(map_info[i].coord)
    #     print(map_info[i].openings)


failed_counts = 0
which_failed = []
for i in range(test_cases):
    for y in range(1, y_length+1):
        for x in range(1, x_length+1):
            if map_maze(storing_array[i], (y, x)) != x_length*y_length:
                failed_counts = failed_counts + 1
                if i not in which_failed:
                    which_failed.append(i)

print(failed_counts)
print(which_failed)
