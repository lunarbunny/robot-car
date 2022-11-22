#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "mappinggod.h"
#include "pico/stdlib.h"
#include "../motor/motor.h"
#include "../motor/pid.h"
#define ONEUNIT 27

void moveTo(int *, int *, int *, int, int);
void moveDirection(mapping_struct *, int *, int);

#endif