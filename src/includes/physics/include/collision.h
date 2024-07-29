#ifndef COLLISION_H
#define COLLISION_H
#include "types.h"

void check_path_collision(const object_t* obj_1, const object_t* obj_2, double* ret_collision_x, double* ret_collision_y);

#endif // COLLISION_H