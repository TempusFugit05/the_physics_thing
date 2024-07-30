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

void advance_object(object_t* obj, double time)
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

void check_collisions(registry_t* registry, object_t* obj_to_check, int object_index, double* ret_time_to_hit, object_t** ret_obj_to_hit, double time)
{
    double time_until_collision_x = 0;
    double time_until_collision_y = 0;

    double time_to_hit = NAN; // Smallest time to collision
    object_t* obj_to_hit = NULL; // Object to be hit first

    for (unsigned int n = object_index + 1; n < object_index + registry->num_members; n++)
    { // Check the collision between current object and the rest
        object_t* obj2 = get_object(registry, n % (registry->num_members));
        if (obj_to_check->last_hit != obj2)
        { // If the two objects didn't collide on the previous tick, check their collision (this is done to prevent two collisions cancelling each other out)
        
            check_path_collision(obj_to_check, obj2, &time_until_collision_x, &time_until_collision_y); // Get time for collision on both axis

            if((time_until_collision_x > 0 &&  time_until_collision_y >= 0) &&
                fabs(((fabs(time_until_collision_x) == NAN) ? time_until_collision_x : 0) - ((fabs(time_until_collision_y) == NAN) ? time_until_collision_y : 0)) <= COLLISION_EPSILON &&
                time_until_collision_x <= time)
            { // Check if collision accurs on both axis and determin which objects will hit first           
                if (obj_to_hit == NULL)
                {
                    time_to_hit = time_until_collision_x;
                    obj_to_hit = obj2;
                }
                
                else if (time_until_collision_x < time_to_hit)
                {
                    time_to_hit = time_until_collision_x;
                    obj_to_hit = obj2;
                }
            }
        }

        else
        {
            obj_to_check->last_hit = NULL;
            obj2->last_hit = NULL;
        }
    }

    if (obj_to_hit != NULL)
    {
        // #ifdef ENABLE_LOGGING
        // extern FILE* file;
        // extern double current_time;
        // char dump1[128];
        // char dump2[128];
        // dump_obj_info(obj_to_check, dump1, 128);
        // dump_obj_info(obj_to_hit, dump2, 128);
        // fprintf(file, "%lf: %s HIT %s!\nt %lf, tx %lf, ty %lf\n%s\n%s\n\n", current_time, obj_to_check->name, obj_to_hit->name, time, time_until_collision_x, time_until_collision_y, dump1, dump2);
        // #endif
        obj_to_check->last_hit = obj_to_hit;
        obj_to_hit->last_hit = obj_to_check;
    }
    
    *ret_obj_to_hit = obj_to_hit;
    *ret_time_to_hit = time_to_hit;
}

static inline void calculate_collision(object_t* obj1, object_t* obj2)
{
    double temp_vel_x = obj1->velocity.x;
    double temp_vel_y = obj1->velocity.y;

    obj1->velocity.x = get_collision_speed(obj1->velocity.x, obj1->mass, obj2->velocity.x, obj2->mass);
    obj1->velocity.y = get_collision_speed(obj1->velocity.y, obj1->mass, obj2->velocity.y, obj2->mass);

    obj2->velocity.x = get_collision_speed(obj2->velocity.x, obj2->mass, temp_vel_x, obj1->mass);
    obj2->velocity.y = get_collision_speed(obj2->velocity.y, obj2->mass, temp_vel_y, obj2->mass);
}

void run_iteration(registry_t* registry, double time)
{
    for (unsigned int i = 0; i < registry->num_members; i++)
    {

        object_t* obj_1 = get_object(registry, i); // Current object

        for (unsigned int n = i + 1; n < registry->num_members - i; n++)
        {
            object_t* obj_2 = get_object(registry, n); // Object to interact with

            double distance = get_distance(obj_1->position, obj_2->position);
            double force = get_electric_force(obj_1->charge, obj_2->charge, distance);
            
            double angle = asin((obj_1->position.y - obj_2->position.y) / distance); 
            
            double force_x = force * cos(angle);
            double force_y = force * sin(angle);
            
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