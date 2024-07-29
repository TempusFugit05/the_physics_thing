#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "pthread.h"
#include "semaphore.h"
#include "raylib.h"

#include "engine.h"

#ifdef ENABLE_LOGGING
FILE* file;
#endif

double current_time = 0;

void create_default_objects(registry_t* registry)
{
    object_t ball_1 = 
    {
        .position = {.x = 3, .y = pixels_to_meter(SCREEN_SIZE_X/2)},
        .velocity = {.x = -100000, .y = 0},
        .acceleration = {.x = 0, .y = 0},
        .mass = 100,
        .size = 1,
        .color = {.a = 255, .r = 255, .g = 0, .b = 0},
        .name = "Red",
    };

    object_t ball_2 =
    {
        .position = {.x = 2, .y = pixels_to_meter(SCREEN_SIZE_X/2)},
        .velocity = {.x = 0, .y = 0},
        .acceleration = {.x = 0, .y = 0},
        .mass = 10,
        .size = 1,
        .color = {.a = 255, .r = 0, .g = 255, .b = 0},
        .name = "Green",
    };

    object_t ball_3 =
    {
        .position = {.x = 1, .y = pixels_to_meter(SCREEN_SIZE_X/2)},
        .velocity = {.x = 0, .y = 0},
        .acceleration = {.x = 0, .y = 0},
        .mass = 1,
        .size = 1,
        .color = {.a = 255, .r = 0, .g = 0, .b = 255},
        .name = "Blue",
    };


    object_t ball_4 =
    {
        .position = {.x = 0.5, .y = pixels_to_meter(SCREEN_SIZE_X/2)},
        .velocity = {.x = 0, .y = 0},
        .acceleration = {.x = 0, .y = 0},
        .mass = INFINITY,
        .size = 0.001,
        .color = {.a = 255, .r = 255, .g = 0, .b = 255},
        .name = "Border_left",
    };

    object_t ball_5 =
    {
        .position = {.x = 100, .y = pixels_to_meter(SCREEN_SIZE_X/2)},
        .velocity = {.x = 0, .y = 0},
        .acceleration = {.x = 0, .y = 0},
        .mass = INFINITY,
        .size = 0.001,
        .color = {.a = 255, .r = 255, .g = 0, .b = 255},
        .name = "Border_right",
    };

    create_object(registry, &ball_1);
    create_object(registry, &ball_2);
    create_object(registry, &ball_3);
    create_object(registry, &ball_4);
    create_object(registry, &ball_5);

}

void* physics_thread(void* args)
{
    thread_info* info = (thread_info*)args;
    
    registry_t main_registry; // Registry onto which all changes to objects are applied
    registry_t snapshot_registry; // Reference registry for interaction calculations at a point in time
    
    create_default_objects(&main_registry);
    create_default_objects(&snapshot_registry);

    info->objects = &main_registry; // Send registry to graphics thread

    double time_delta;

    sem_wait(info->graphics_ready_sem); // Wait for graphics to finish initializing

    #ifdef ENABLE_LOGGING
    file = fopen("./results.txt", "w");    
    if (file == NULL)
    {
        printf("Can't open file!");
        exit(1);
    }
    #endif

    while(1)
    {
        start_iteration(); // Start iteration timer

        run_iteration(&main_registry, &snapshot_registry, time_delta);

        for (unsigned int i = 0; i < snapshot_registry.num_members; i++)
        { // Copy objects to snapshot 
            *get_object(&snapshot_registry, i) = *get_object(&main_registry, i);
        }
        
        current_time += time_delta;
        
        time_delta = end_iteration(); // End iteration and send thread to sleep
    }
}

void* graphics_thread(void* args)
{
    InitWindow(SCREEN_SIZE_X, SCREEN_SIZE_Y, "Test!");
    SetTargetFPS(60);
    
    thread_info* info = (thread_info*)args; // Information shared between threads

    double time_delta;

    sem_post(info->graphics_ready_sem); // Signal that graphics are ready

    while (!WindowShouldClose())
    {
        time_delta = info->iteration_time;

        BeginDrawing();

        for (unsigned int i = 0; i < info->objects->num_members; i++)
        {
            object_t obj = *get_object(info->objects, i);
            DrawCircle(meters_to_pixel(obj.position.x), SCREEN_SIZE_Y -  meters_to_pixel(obj.position.y), meters_to_pixel(obj.size), obj.color); // Draw objects
        }
        
        ClearBackground(WHITE);

        DrawText(TextFormat("%lf", time_delta), 10, 10, 20, BLACK);

        // DrawText(TextFormat("Gravity (m/s^2): %f\n\nPosition (m)\nX: %f\nY: %f\n\nVelocity (m/s)\nX: %f\nY: %f",
        // (double)GRAVITATIONAL_CONSTATNT_MPS, ball.position.x, ball.position.y, ball.velocity.x, ball.velocity.y), 10, 10, 16, BLACK); // Display all the ball's physical values

        EndDrawing();
    }

    CloseWindow();
    info->program_closed = true;
    pthread_exit(NULL);
}

int main()
{
    pthread_t graphics_thread_id;
    pthread_t physics_thread_id;
    
    sem_t graphics_ready_semaphore; // Semaphore to signify that the graphics are initialized before starting physics simulation
    sem_init(&graphics_ready_semaphore, 0, 0);

    thread_info info = {.graphics_ready_sem = &graphics_ready_semaphore, .program_closed = false, .mouse_lock = false, .iteration_time = 0, .objects = NULL}; // Data to be shared with both threads
    
    pthread_create(&physics_thread_id, NULL, physics_thread, &info);
    pthread_create(&graphics_thread_id, NULL, graphics_thread, &info);

    while (!info.program_closed){}
    
    pthread_cancel(physics_thread_id); // Kill physics thread
    
    return 1;
}

