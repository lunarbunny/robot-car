import sys
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import make_interp_spline

graph_times = []
graph_speeds_left = []
graph_speeds_right = []
graph_setpoints = []

if len(sys.argv) > 1:
    file = open(sys.argv[1])
else:
    file = open('pid.txt')

for line in file:
    if line.startswith('T'):
        columns = line.split('|')
        timeSec = float(columns[0][3:-2])
        speed = float(columns[2][8:-1])
        setpoint = float(columns[1][5:-1])
        
        motorSide = columns[2][5]
        if motorSide == 'L':
            graph_speeds_left.append(speed)
            graph_times.append(timeSec)
            graph_setpoints.append(setpoint)
        elif motorSide == 'R':
            graph_speeds_right.append(speed)

        print(timeSec, speed, setpoint)

# Smooth left pid curve
X_Y_Spline = make_interp_spline(graph_times, graph_speeds_left)
X_ = np.linspace(min(graph_times), max(graph_times), 500)
Y_ = X_Y_Spline(X_)
plt.plot(X_, Y_, label="Left motor speed (smooth)", color="red")

# Smooth right pid curve
X_Y_Spline = make_interp_spline(graph_times, graph_speeds_right)
X_ = np.linspace(min(graph_times), max(graph_times), 500)
Y_ = X_Y_Spline(X_)
plt.plot(X_, Y_, label="Right motor speed (smooth)", color="blue")

# plot points
#plt.plot(graph_times, graph_speeds_left, label="Left motor speed", color="red")
#plt.plot(graph_times, graph_speeds_right, label="Right motor speed", color="blue")
plt.plot(graph_times, graph_setpoints, label="Setpoint", color = "black")

plt.xlabel('time (s)')
plt.ylabel('speed (cm/s)')
plt.title('PID')
plt.legend()

plt.show(block=True)
