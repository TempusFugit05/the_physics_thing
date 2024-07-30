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
    obj->acceleration.x = get_second_law_acceleration(obj->force.x, obj->mass);
    obj->acceleration.y = get_second_law_acceleration(obj->force.y, obj->mass);

    obj->force.x = 0;
    obj->force.y = 0;

    obj->position.x += get_position(obj->velocity.x, obj->acceleration.x, time);
    obj->position.y += get_position(obj->velocity.y, obj->acceleration.y, time);

    obj->velocity.x += get_velocity(obj->acceleration.x, time);
    obj->velocity.y += get_velocity(obj->acceleration.y, time);

    printf("%f, %f, %f\n", obj->force.y, obj->mass, obj->acceleration.y);
}

void check_collisions(registry_t* registry, object_t* obj_to_check, int object_index, double* ret_time_to_hit, object_t** ret_obj_to_hit, double time)
{
    double time_until_collision_x = 0;
    double time_until_collision_y = 0;

    // #ifdef ENABLE_LOGGING
    // int num_hits = 0;
    // #endif

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

                // #ifdef ENABLE_LOGGING
                // num_hits++;
                // extern FILE* file;
                // fprintf(file, "Hit x %d!\n", num_hits);
                // #endif  

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
            
            obj_1->force.x += force_x;
            obj_1->force.y += force_y;

            obj_2->force.x -= force_x;
            obj_2->force.y -= force_y;
        }
    }

    for (unsigned int i = 0; i < registry->num_members; i++)
    {
        object_t* obj_1 = get_object(registry, i); // Current object

        advance_object(obj_1, time);
    }
}
//         object_t* current_obj = get_object(registry, i % registry->num_members); // Object to check against the rest

//         double time_to_hit = 0; // Smallest time to collision
        
//         double sub_tick_time = time; // This time will be used to calculate the new positions of objects which collide multiple times in a tick

//         object_t* obj_to_hit = NULL; // Object to be hit first
//         object_t* last_object_hit = NULL;

//         check_collisions(registry, current_obj, i, &time_to_hit, &obj_to_hit, time);

//         while (obj_to_hit != NULL)
//         {
//             last_object_hit = obj_to_hit;
            
//             advance_object(current_obj, time_to_hit);

//             calculate_collision(current_obj, obj_to_hit);

//             sub_tick_time -= time_to_hit;

//             check_collisions(registry, current_obj, i, &time_to_hit, &obj_to_hit, sub_tick_time);
//         }
        
//         if (obj_to_hit == NULL)
//         {
//             if (last_object_hit != NULL)
//             {
//                 #ifdef ENABLE_LOGGING
//                 double collision_x;
//                 double collision_y;
//                 check_path_collision(current_obj, last_object_hit, &collision_x, &collision_y);

//                 position_t temp_current_obj_pos = current_obj->position;
//                 position_t position_delta_before = {.x = temp_current_obj_pos.x - last_object_hit->position.x, .y = temp_current_obj_pos.y - last_object_hit->position.y};
//                 #endif
                
//                 advance_object(current_obj, sub_tick_time);
                
//                 #ifdef ENABLE_LOGGING
//                 temp_current_obj_pos = current_obj->position;
//                 position_t position_delta_after = {.x = temp_current_obj_pos.x - last_object_hit->position.x, .y = temp_current_obj_pos.y - last_object_hit->position.y};
//                 extern FILE* file;
//                 extern double current_time;

//                 if (signbit(position_delta_before.x) != signbit(position_delta_after.x))
//                 {
//                     char dump1[128];
//                     char dump2[128];
//                     dump_obj_info(current_obj, dump1, 128);
//                     dump_obj_info(last_object_hit, dump2, 128);
//                     fprintf(file, "%lf: %s PASSED %s!\nt %lf, tx %lf, ty %lf\n%s\n%s\n\n", current_time, current_obj->name, last_object_hit->name, time, collision_x, collision_y, dump1, dump2);
//                 }

//                 // if (signbit(position_delta_before.y) != signbit(position_delta_after.y))
//                 // { 
//                 //     fprintf(file, "%s PASSED %s\n%lf: t %lf,  time x %lf, time y %lf, vx %lf, px %lf\n", 
//                 //     current_obj->name, last_object_hit->name, current_time, time, collision_x, collision_y, current_obj->velocity.x, current_obj->position.x,);
//                 // }
//                 #endif

//                 last_object_hit = NULL;
//             }

//             else
//             {
//                 advance_object(current_obj, sub_tick_time);
//             }
//         }
//             // #ifdef ENABLE_LOGGING
//             // if (current_obj->position.x <  0.5 || current_obj->position.x > 3.5)
//             // {
//             //     extern FILE* file;
//             //     fprintf(file, "BAD! %lf\n", current_obj->position.x);
//             // }
//             // #endif
//     }
// }