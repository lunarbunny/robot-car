from pyamaze import maze
x_length = 4
y_length = 5
m = maze()
m.CreateMaze(loadMaze="maze.csv")
m.run()
maze = m.maze_map

start = (3,3)

with open('conversion.txt', 'w') as f:
    for y in range(1, x_length+1):
        for x in range(1, y_length+1):
            f.write(f"{x-start[0]},{y-start[1]}\n")
            f.write(f"{maze[(y,x)].get('N')},")
            f.write(f"{maze[(y,x)].get('E')},")
            f.write(f"{maze[(y,x)].get('S')},")
            f.write(f"{maze[(y,x)].get('W')}\n")            
