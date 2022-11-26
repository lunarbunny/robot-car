#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "mappinggod.h"

// State 3 - Moving a unit (uses shared_buffer direction to move)
void moveDirection(mapping_struct *, int *, int *, int *);
// State 6 - Moving all the way (from navigation)
void moveLong(compute_path, int *, int *);

#endif