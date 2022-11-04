from pyamaze import maze

maze_input = {(1, 1): {'E': 1, 'W': 0, 'N': 0, 'S': 1}, (2, 1): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (3, 1): {'E': 1, 'W': 0, 'N': 1, 'S': 1}, (4, 1): {'E': 1, 'W': 0, 'N': 1, 'S': 0}, (1, 2): {'E': 1, 'W': 1, 'N': 0, 'S': 1}, (2, 2): {'E': 1, 'W': 0, 'N': 1, 'S': 1}, (3, 2): {'E': 0, 'W': 1, 'N': 1, 'S': 0}, (4, 2): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (1, 3): {'E': 1, 'W': 1, 'N': 0, 'S': 0}, (2, 3): {'E': 1, 'W': 1, 'N': 0, 'S': 1}, (3, 3): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 3): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (1, 4): {'E': 1, 'W': 1, 'N': 0, 'S': 1}, (2, 4): {'E': 0, 'W': 1, 'N': 1, 'S': 0}, (3, 4): {'E': 0, 'W': 0, 'N': 0, 'S': 1}, (4, 4): {'E': 1, 'W': 1, 'N': 1, 'S': 0}, (1, 5): {'E': 0, 'W': 1, 'N': 0, 'S': 1}, (2, 5): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (3, 5): {'E': 0, 'W': 0, 'N': 1, 'S': 1}, (4, 5): {'E': 0, 'W': 1, 'N': 1, 'S': 0}}
y_length = 4   # left field
x_length = 5  # right field

with open('maze.csv', 'w') as f:
  f.write(f"  cell  ,E,W,N,S\n")
  for x in range(1, x_length+1):
    for y in range(1, y_length+1):
        f.write(f"\"({y}, {x})\",")
        f.write(f"{maze_input[(y,x)].get('E')},")
        f.write(f"{maze_input[(y,x)].get('W')},")
        f.write(f"{maze_input[(y,x)].get('N')},")
        f.write(f"{maze_input[(y,x)].get('S')}\n") 

m = maze()
m.CreateMaze(loadMaze="maze.csv")
m.run()