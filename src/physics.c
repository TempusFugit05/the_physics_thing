#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "engine.h"
#include "program_threads.h"

void start_iteration(struct timespec* start_time)
{
    /*Store the time when this function was called*/
    clock_gettime(CLOCK_REALTIME, start_time); // Start timer
}

psize_t end_iteration(float simulation_speed, unsigned int iterations_per_second, unsigned int* iterations_to_run, struct timespec* start_time)
{
    /*End the iteration, go to sleep for set amount of time and return the amount of time the origram slept*/
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time); // Stop timer

    time_t interation_time = (end_time.tv_nsec - start_time->tv_nsec + ((start_time->tv_sec < end_time.tv_sec) ? 1000 : 0)) / 1000000; // Get the time delta between the beginning and end of iteration
    // time_t base_tick_time = (1000000 / (ITERATIONS_PER_SECOND * simulation_speed)); // Target time for iteration
    
    time_t time_to_sleep = (TIME_TO_SLEEP - interation_time > 0) ? TIME_TO_SLEEP - interation_time : 1;
    *iterations_to_run = (time_to_sleep * iterations_per_second / 1000000) * simulation_speed;

    usleep(time_to_sleep);

    if (signbit(simulation_speed))
    {
        return -(double)1 / (double)ITERATIONS_PER_SECOND;
    }
    
    return (double)1 / (double)ITERATIONS_PER_SECOND;  // Time the program slept for, converted from microseconds to seconds
}

void create_default_objects(registry_t* registry)
{
    object_t ball_1 =
    {
        .position = {.x = 100, .y = 0},
        .velocity = {.x = 0, .y = 0},
        .acceleration = {.x = 0, .y = 0},
        .mass = 10000,
        .size = 10,
        .charge = 1,
        .color = {.a = 255, .r = 255, .g = 0, .b = 0},
        .name = "Red",
        .is_target = true
    };

    create_object(registry, &ball_1);
    ball_1.position.y = -100;
    ball_1.position.x = 500;
    ball_1.charge = -1;
    ball_1.velocity.y = 25;
    create_object(registry, &ball_1);
}

void* physics_thread(void* args)
{
    thread_info* info = (thread_info*)args;
    
    registry_t main_registry; // Registry onto which all changes to objects are applied
    registry_t snapshot_registry; // Reference registry for interaction calculations at a point in time
    
    create_default_objects(&main_registry);
    create_default_objects(&snapshot_registry);

    info->objects = &main_registry; // Send registry to graphics thread

    double time_delta = 0;
    double current_time = 0;
    unsigned int iter_to_perform = 0;

    struct timespec start_time;

    sem_wait(info->graphics_ready_sem); // Wait for graphics to finish initializing

    #ifdef ENABLE_LOGGING
    file = fopen("./results.txt", "w");    
    if (file == NULL)
    {
        printf("Can't open file!");
        exit(1);
    }
    #endif

    while(!info->program_closed)
    {
        start_iteration(&start_time); // Start iteration timer

        for (unsigned int i = 0; i < iter_to_perform; i++)
        {
            if (info->is_paused)
            {
                break;
            }
            
            run_iteration(&main_registry, time_delta);

            if (sem_post(info->request_objects_sem) == 0)
            {
                for (unsigned int i = 0; i < snapshot_registry.num_members; i++)
                { // Copy objects to snapshot 
                    *get_object(&snapshot_registry, i) = *get_object(&main_registry, i);
                }        
            }
            
            current_time += time_delta;
            
            info->simulation_time = current_time;
            info->iteration_num += 1;
        }

        time_delta = end_iteration(info->simulation_speed, ITERATIONS_PER_SECOND, &iter_to_perform, &start_time); // End iteration and send thread to sleep
    }

    pthread_exit(NULL);
}
