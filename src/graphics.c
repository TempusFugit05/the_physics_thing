#include <pthread.h>
#include <locale.h>
#include "raylib.h"

#include "program_threads.h"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui/raygui.h"
#include "engine.h"

void get_iteration_average(unsigned int* ret_average, unsigned int current_iteration_num)
{
    static unsigned int prev_iteration_num = 0;
    static unsigned int iteration_sum = 0;
    static float time_sum = 0;
    static unsigned int frame_count = 0;

    frame_count++;
    time_sum += GetFrameTime();
    iteration_sum += current_iteration_num - prev_iteration_num;
    prev_iteration_num = current_iteration_num;


    if (frame_count % 15 == 0)
    {
        *ret_average = (int)roundf((float)iteration_sum / time_sum);
        iteration_sum = 0;
        time_sum = 0;
    }
}

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

    unsigned int iter_per_second = 0;

    while (!WindowShouldClose())
    {
        get_iteration_average(&iter_per_second, info->iteration_num);        

        sem_trywait(info->request_objects_sem); // Request objects

        if (IsKeyPressed(KEY_SPACE))
        {
            info->is_paused = !info->is_paused;
        }
        
        /*Camera zoom*/
        float scroll_wheel = GetMouseWheelMove();
        if (scroll_wheel != 0)
        {
            Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            camera.target = mouse_world_pos;

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
        
        slider_value_changed = GuiSlider(slider_bounds, "-", "+", &simulation_speed, MIN_SIMULATION_SPEED, MAX_SIMULATION_SPEED);
        info->simulation_speed = simulation_speed;

        DrawText(TextFormat("%d:%d:%d:%d", (int)(info->simulation_time / (60 * 24)) % 60, (int)(info->simulation_time / 60) % 60, (int)(info->simulation_time) % 60, (int)(info->simulation_time * 1000) % 1000), 10, 10, 20, BLACK);
        DrawText(TextFormat("Speed: %4.2fX", simulation_speed), slider_bounds.x + 200 - (MeasureTextEx(GetFontDefault(), TextFormat("%f", simulation_speed), 10, GetFontDefault().glyphPadding).x / 2), slider_bounds.y + 10, 10, BLACK);
        setlocale(LC_NUMERIC, "");
        DrawText(TextFormat("Iterations per second: %'d", iter_per_second) , 10, 40, 20, BLACK);

        EndDrawing();
    }

    /*Deinit graphics thread*/
    CloseWindow();
    info->program_closed = true;
    pthread_exit(NULL);
}
