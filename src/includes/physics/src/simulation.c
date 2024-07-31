#include "../include/simulation.h"
#include "../include/equations.h"
#include "../include/types.h"
#include "../include/collision.h"
#include "../include/configs.h"
#include "../../utils/object_manager.h"
#include "../../utils/util_functions.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

void advance_object(object_t* obj, psize_t time)
{
    // printf("%s: %f, %f\n", obj->name, obj->force.x, obj->force.y);

    obj->acceleration.x = get_second_law_acceleration(obj->force.x, obj->mass);
    obj->acceleration.y = get_second_law_acceleration(obj->force.y, obj->mass);

    obj->force.x = 0;
    obj->force.y = 0;

    obj->velocity.x += get_velocity(obj->acceleration.x, time);
    obj->velocity.y += get_velocity(obj->acceleration.y, time);

    obj->position.x += get_position(obj->velocity.x, obj->acceleration.x, time);
    obj->position.y += get_position(obj->velocity.y, obj->acceleration.y, time);
}

// void check_collisions(registry_t* registry, object_t* obj_to_check, int object_index, psize_t* ret_time_to_hit, object_t** ret_obj_to_hit, psize_t time)
// {
//     psize_t time_until_collision_x = 0;
//     psize_t time_until_collision_y = 0;

//     psize_t time_to_hit = NAN; // Smallest time to collision
//     object_t* obj_to_hit = NULL; // Object to be hit first

//     for (unsigned int n = object_index + 1; n < object_index + registry->num_members; n++)
//     { // Check the collision between current object and the rest
//         object_t* obj2 = get_object(registry, n % (registry->num_members));
//     *ret_obj_to_hit = obj_to_hit;
//     *ret_time_to_hit = time_to_hit;
//     }
// }

static inline void calculate_collision(object_t* obj1, object_t* obj2)
{
    psize_t temp_vel_x = obj1->velocity.x;
    psize_t temp_vel_y = obj1->velocity.y;

    obj1->velocity.x = get_collision_speed(obj1->velocity.x, obj1->mass, obj2->velocity.x, obj2->mass);
    obj1->velocity.y = get_collision_speed(obj1->velocity.y, obj1->mass, obj2->velocity.y, obj2->mass);

    obj2->velocity.x = get_collision_speed(obj2->velocity.x, obj2->mass, temp_vel_x, obj1->mass);
    obj2->velocity.y = get_collision_speed(obj2->velocity.y, obj2->mass, temp_vel_y, obj2->mass);
}

void run_iteration(registry_t* registry, psize_t time)
{
    for (unsigned int i = 0; i < registry->num_members; i++)
    {

        object_t* obj_1 = get_object(registry, i); // Current object

        for (unsigned int n = i + 1; n < registry->num_members - i; n++)
        {
            object_t* obj_2 = get_object(registry, n); // Object to interact with

            psize_t distance = get_distance(obj_1->position, obj_2->position);
            psize_t force = get_electric_force(obj_1->charge, obj_2->charge, distance);
            
            psize_t angle = asin((obj_1->position.y - obj_2->position.y) / distance); 
            
            psize_t force_x = force * cos(angle);
            psize_t force_y = force * sin(angle);
            
            if (signbit(obj_1->position.x - obj_2->position.x))
            {
                force_x = -force_x;
            }
            
            obj_1->force.x += force_x;
            obj_2->force.x -= force_x;

            obj_1->force.y += force_y;
            obj_2->force.y -= force_y;
        }
    }

    for (unsigned int i = 0; i < registry->num_members; i++)
    {
        object_t* obj_1 = get_object(registry, i); // Current object

        advance_object(obj_1, time);
    }
}