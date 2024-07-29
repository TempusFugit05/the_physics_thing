#include "../include/collision.h"
#include "../include/types.h"
#include "../include/configs.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static inline double velocity_collision(const double pos_1, const double vel_1, const double pos_2, const double vel_2)
{
    return (pos_2 - pos_1) / (vel_1 - vel_2);
}

static inline double acceleration_collision(const double pos_1, const double vel_1, const double acc_1, const double pos_2, const double vel_2, const double acc_2)
{
    return ((vel_2 - vel_1) - sqrt(((vel_1 - vel_2) * (vel_1 - vel_2)) - 2 * ((acc_1 - acc_2) * (pos_1 - pos_2)))) / (acc_1 - acc_2);
}

void check_path_collision(const object_t* obj_1, const object_t* obj_2, double* ret_collision_x, double* ret_collision_y)
{
    /*Checks weather or not an object will intersect another during it's movement. Return true if objects will collide this tick.*/

    double collision_x = 0;
    double collision_y = 0;
    
    if (obj_1->acceleration.x == obj_2->acceleration.x)
    {
        collision_x = velocity_collision(obj_1->position.x, obj_1->velocity.x, obj_2->position.x, obj_2->velocity.x);
    }
    
    else
    {
        collision_x = acceleration_collision(obj_1->position.x, obj_1->velocity.x, obj_1->acceleration.x, obj_2->position.x, obj_2->velocity.x, obj_2->acceleration.x);
    }
    
    if (obj_1->acceleration.y == obj_2->acceleration.y)
    {
        collision_y = velocity_collision(obj_1->position.y, obj_1->velocity.y, obj_2->position.y, obj_2->velocity.y);
    }
    
    else
    {
        collision_y = acceleration_collision(obj_1->position.y, obj_1->velocity.y, obj_1->acceleration.y, obj_2->position.y, obj_2->velocity.y, obj_2->acceleration.y);
    }
    
    *ret_collision_x = collision_x;
    *ret_collision_y = collision_y;
}
