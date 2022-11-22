#include "pico/stdlib.h"
#include "motor/motor.h"
#include "motor/pid.h"

void moveTo(int *current_x, int *current_y, int *direction, int to_x, int to_y)
{

    int intended_dir, turn_operation;

    // Move south
    if (to_y - *current_y == 1)
        intended_dir = 2;
    // Move north
    else if (to_y - *current_y == -1)
        intended_dir = 0;
    // Move east
    else if (to_x - *current_x == 1)
        intended_dir = 1;
    // Move west
    else if (to_x - *current_x == -1)
        intended_dir = 3;

    turn_operation = direction - intended_dir;
    if (turn_operation < 0)
        turn_operation += 4;

    switch (turn_operation)
    {
    case 0:
        MoveFoward(CMtoSteps(ONEUNIT));
        *current_y--;
        break;
    case 1:
        TurnRight();
        *direction++;
        MoveFoward(CMtoSteps(ONEUNIT));
        current_x++;
        break;
    case 2:
        MoveBackward(CMtoSteps(ONEUNIT));
        *current_y++;
        break;
    case 3:
        TurnLeft();
        *direction--;
        MoveFoward(CMtoSteps(ONEUNIT));
        current_x--;
        break;
    }
}