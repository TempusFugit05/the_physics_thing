#include <pthread.h>

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

    position_t camera_pos = {.x = 0, .y = 0};

    sem_post(info->graphics_ready_sem); // Signal that graphics are ready
    
    Rectangle slider_bounds = {.x = (SCREEN_SIZE_X - 200) / 2, .y = 20, .width = 400, .height = 10};
    
    float pixels_per_meter = 10; // Zoom factor
    
    float simulation_speed = DEFAULT_SIMULATION_SPEED;

    int slider_value_changed = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(WHITE);
            
        float scroll_wheel = GetMouseWheelMove();

        if (scroll_wheel < 0)
        {
            if (pixels_per_meter > 0)
            {
                pixels_per_meter /= 1.05;
            }
        }

        else if (scroll_wheel > 0)
        {
            pixels_per_meter *= 1.05;
        }

        for (unsigned int i = 0; i < info->objects->num_members; i++)
        {
            object_t obj = *get_object(info->objects, i);
            DrawCircle(meters_to_pixel(obj.position.x, pixels_per_meter) + camera_pos.x, SCREEN_SIZE_Y -  meters_to_pixel(obj.position.y, pixels_per_meter) + camera_pos.y, meters_to_pixel(obj.size, pixels_per_meter), obj.color); // Draw objects
        }
        
        DrawText(TextFormat("%lf", info->iteration_time), 10, 10, 20, BLACK);
        slider_value_changed = GuiSlider(slider_bounds, "-", "+", &simulation_speed, MIN_SIMULATION_SPEED, MAX_SIMULATION_SPEED);
        info->simulation_speed = simulation_speed;
        
        DrawText(TextFormat("%f", simulation_speed), slider_bounds.x + 200 - (MeasureTextEx(GetFontDefault(), TextFormat("%f", simulation_speed), 10, GetFontDefault().glyphPadding).x / 2), slider_bounds.y + 10, 10, BLACK);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && slider_value_changed == 0)
        {
            Vector2 mouse_delta = GetMouseDelta();
            camera_pos.x += mouse_delta.x;
            camera_pos.y += mouse_delta.y;
        }

        EndDrawing();
    }

    CloseWindow();
    info->program_closed = true;
    pthread_exit(NULL);
}
