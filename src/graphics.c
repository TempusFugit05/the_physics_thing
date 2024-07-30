#include "graphics.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui/raygui.h"
#include "engine.h"

void* graphics_thread(void* args)
{
    InitWindow(SCREEN_SIZE_X, SCREEN_SIZE_Y, "Test!");
    SetTargetFPS(60);
    
    thread_info* info = (thread_info*)args; // Information shared between threads

    double time_delta;

    sem_post(info->graphics_ready_sem); // Signal that graphics are ready
    
    Rectangle slider_bounds = {.x = 10, .y = 20, .width = 100, .height = 10};
    

    
    GuiSlider(slider_bounds, "", "", &PIXELS_PER_METER, 1, 100);

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
