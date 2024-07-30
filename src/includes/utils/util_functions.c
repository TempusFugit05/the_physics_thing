#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include "util_functions.h"
#include "includes/physics/include/configs.h"
#include "includes/physics/include/types.h"

#include <time.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

void dump_obj_info(const object_t* obj, char* string, const unsigned int string_size)
{
    snprintf(string, string_size, "%s: ax %lf, ay %lf, vx %lf, vy %lf, px %lf, py %lf", obj->name, obj->acceleration.x, obj->acceleration.y, obj->velocity.x, obj->velocity.y, obj->position.x, obj->position.y);
} 

inline int meters_to_pixel(const double meters, const double pixels_per_meter)
{
    return (int)round(meters*pixels_per_meter);
}

inline double pixels_to_meter(const int pixels, const double pixels_per_meter)
{
    return (double)pixels/pixels_per_meter;
}

static struct timespec start_time, end_time; // Times at the start and end of the current iteration to calculate how much time it took to complete

inline void start_iteration()
{
    /*Store the time when this function was called*/
    clock_gettime(CLOCK_REALTIME, &start_time); // Start timer
}

inline double end_iteration()
{
    /*End the iteration, go to sleep for set amount of time and return the amount of time the origram slept*/
    clock_gettime(CLOCK_REALTIME, &end_time); // Stop timer

    time_t interation_time = (end_time.tv_nsec - start_time.tv_nsec + ((start_time.tv_sec < end_time.tv_sec) ? 1000 : 0)) / 1000000; // Get the time delta between the beginning and end of iteration
    static time_t base_tick_time = (1000000 / (ITERATIONS_PER_SECOND * TIME_FACTOR)); // Target time for iteration
    
    time_t time_to_sleep = (base_tick_time - interation_time < MINIMUM_SLEEP_TIME) ? MINIMUM_SLEEP_TIME : base_tick_time - interation_time;

    usleep(time_to_sleep);

    return (double)1 / (double)ITERATIONS_PER_SECOND; // Time the program slept for, converted from microseconds to seconds
}