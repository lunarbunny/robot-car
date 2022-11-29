class grid_struct:
    def __init__(self, coord, openings):
        self.coord = coord
        self.openings = openings

maze = {(1, 1): {'E': 1, 'W': 0, 'N': 0, 'S': 0}, (2, 1): {'E': 1, 'W': 0, 'N': 0, 'S': 1}, (3, 1): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 1): {'E': 1, 'W': 0, 'N': 1, 'S': 0}, (1, 2): {'E': 1, 'W': 1, 'N': 0, 'S': 1}, (2, 2): {'E': 0, 'W': 1, 'N': 1, 'S': 1}, (3, 2): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 2): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (1, 3): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (2, 3): {'E': 1, 'W': 0, 'N': 0, 'S': 1}, (3, 3): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 3): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (1, 4): {'E': 1, 'W': 1, 'N': 0, 'S': 1}, (2, 4): {'E': 0, 'W': 1, 'N': 1, 'S': 1}, (3, 4): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 4): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (1, 5): {'E': 0, 'W': 1, 'N': 0, 'S': 1}, (2, 5): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (3, 5): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 5): {'E': 0, 'W': 1, 'N': 1, 'S': 0}}

bot_path = []           # path the bot will store
visited = []            # visited coords list
debug_full_path = []    # for demo only, the full path including backtracking
map_info = []           # stores all grid info, coords and openings

start_pos = (1,1)
current_pos = start_pos

# the bot's starting position specs
visited.append(current_pos)
bot_path.append(current_pos)
debug_full_path.append(current_pos)
map_info = [grid_struct(current_pos, maze[current_pos])]

# the bot will attempt to navigate with preference for north, followed by a clockwise check
if maze[current_pos].get('N') == 1:
    current_pos = (current_pos[0]-1, current_pos[1])
    visited.append(current_pos)
elif maze[current_pos].get('E') == 1:
    current_pos = (current_pos[0], current_pos[1]+1)
    visited.append(current_pos)
elif maze[current_pos].get('S') == 1:
    current_pos = (current_pos[0]+1, current_pos[1])
    visited.append(current_pos)
elif maze[current_pos].get('W') == 1:
    current_pos = (current_pos[0], current_pos[1]-1)
    visited.append(current_pos)

backstepped = False # variable to store if there's no way to go, so the bot must backstep

# keep moving until reached original position
while (current_pos != start_pos or
    (maze[start_pos].get('N') == 1 and (start_pos[0]-1, start_pos[1]) not in visited) or 
    (maze[start_pos].get('E') == 1 and (start_pos[0], start_pos[1]+1) not in visited) or 
    (maze[start_pos].get('S') == 1 and (start_pos[0]+1, start_pos[1]) not in visited) or 
    (maze[start_pos].get('W') == 1 and (start_pos[0], start_pos[1]-1) not in visited)):
# only add to bot_path if the resultant move is not a backtrack
    if not backstepped:
        bot_path.append(current_pos)
        map_info.append(grid_struct(current_pos, maze[current_pos]))   # store info

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

print(start_pos)

print("===\nmap tiles stored: ", len(map_info))
print("visit order: ", debug_full_path)
print("storing order: ", visited)

print("===")
for i in range(len(map_info)):
    print(map_info[i].coord)
    print(map_info[i].openings)