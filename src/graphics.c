#include <pthread.h>

#include "graphics.h"
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui/raygui.h"
#include "engine.h"

void* graphics_thread(void* args)
{
    thread_info* info = (thread_info*)args; // Information shared between threads
    
    InitWindow(SCREEN_SIZE_X, SCREEN_SIZE_Y, "Test!");

    SetTargetFPS(60);
    
    sem_post(info->graphics_ready_sem); // Signal that graphics are ready

    Rectangle slider_bounds = {.x = (SCREEN_SIZE_X - 200) / 2, .y = 20, .width = 400, .height = 10};
        
    float simulation_speed = DEFAULT_SIMULATION_SPEED;

    int slider_value_changed = 0;

    Camera2D camera = {.zoom = 1};

    long int prev_iteration_num = 0;

    while (!WindowShouldClose())
    {
        sem_trywait(info->request_objects_sem); // Request objects

        if (IsKeyPressed(KEY_SPACE))
        {
            info->is_paused = !info->is_paused;
        }
        

        /*Camera zoom*/
        float scroll_wheel = GetMouseWheelMove();
        if (scroll_wheel != 0)
        {        
            if (scroll_wheel < 0)
            {
                camera.zoom /= 1.05;
            }

            else if (scroll_wheel > 0)
            {
                camera.zoom *= 1.05;
            }
        }

        /*Camera pan*/
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && slider_value_changed == 0)
        {
            Vector2 mouse_delta = GetMouseDelta();
            camera.offset.x += mouse_delta.x;
            camera.offset.y += mouse_delta.y;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        BeginMode2D(camera);

        /*Draw objects on screen*/
        for (unsigned int i = 0; i < info->objects->num_members; i++)
        {
            object_t* obj = get_object(info->objects, i);
            
            DrawCircleV((Vector2){obj->position.x, obj->position.y}, obj->size, obj->color); // Draw objects
            DrawCircleLinesV((Vector2){obj->position.x, obj->position.y}, obj->size, BLACK);
        }
        
        EndMode2D();
        
        if (GuiLabelButton((Rectangle){.x = GetScreenWidth() - 39, .y = 11, .width = 14, .height = 14}, ""))
        {
            info->is_paused = !info->is_paused;
        }

        if (!info->is_paused)
        {
            GuiDrawIcon(ICON_PLAYER_PAUSE, GetScreenWidth() - 40, 10, 1, BLACK);
        }
        else
        {
            GuiDrawIcon(ICON_PLAYER_PLAY, GetScreenWidth() - 40, 10, 1, BLACK);
        }
        


        DrawText(TextFormat("%lf", info->simulation_time), 10, 10, 20, BLACK);
        slider_value_changed = GuiSlider(slider_bounds, "-", "+", &simulation_speed, MIN_SIMULATION_SPEED, MAX_SIMULATION_SPEED);
        info->simulation_speed = simulation_speed;
        
        DrawText(TextFormat("Speed: %4.2fX", simulation_speed), slider_bounds.x + 200 - (MeasureTextEx(GetFontDefault(), TextFormat("%f", simulation_speed), 10, GetFontDefault().glyphPadding).x / 2), slider_bounds.y + 10, 10, BLACK);
        DrawText(TextFormat("Iterations per second: %d", (int)((info->iteration_num - prev_iteration_num) / GetFrameTime())), 10, 40, 20, BLACK);
        prev_iteration_num = info->iteration_num;

        EndDrawing();


    }

    /*Deinit graphics thread*/
    CloseWindow();
    info->program_closed = true;
    pthread_exit(NULL);
}
