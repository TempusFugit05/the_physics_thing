#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "pthread.h"
#include "semaphore.h"

#include "engine.h"
#include "program_threads.h"

#ifdef ENABLE_LOGGING
FILE* file;
#endif

int main()
{
    pthread_t graphics_thread_id;
    pthread_t physics_thread_id;
    
    sem_t graphics_ready_semaphore; // Semaphore to signify that the graphics are initialized before starting physics simulation
    sem_init(&graphics_ready_semaphore, 0, 0);

    sem_t request_objects_sem;
    sem_init(&request_objects_sem, 0, 0);

    thread_info info = 
    {
        .graphics_ready_sem = &graphics_ready_semaphore,
        .request_objects_sem = &request_objects_sem,
        .program_closed = false,
        .simulation_time = 0,
        .objects = NULL,
        .simulation_speed = DEFAULT_SIMULATION_SPEED
    }; // Data to be shared with both threads
    
    pthread_create(&physics_thread_id, NULL, physics_thread, &info);
    pthread_create(&graphics_thread_id, NULL, graphics_thread, &info);

    while (!info.program_closed){}

    return 1;
}