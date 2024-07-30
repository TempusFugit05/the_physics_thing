#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <semaphore.h>

// typedef enum
// {
//     DEFAULT,
//     GRAPHICS_READY,
//     MOUSE_LOCK,
//     EXIT,
// } sim_states_t;

typedef struct
{
    double x;
    double y;
} position_t;

typedef struct
{
    double x;
    double y;
} velocity_t;

typedef struct
{
    double x;
    double y;
} acceleration_t;

typedef struct
{
    double x;
    double y;
} force_t;

typedef struct object_t
{
    position_t position;
    velocity_t velocity;
    acceleration_t acceleration;
    force_t force;
    double mass;
    double size;
    Color color;
    double charge;
    char name[32];
    struct object_t* last_hit;
} object_t;

typedef struct member_t
{
    object_t obj;
    struct member_t* next_member;
} member_t;

typedef struct
{
    unsigned int num_members;
    member_t* head;
} registry_t;

typedef struct
{
    registry_t* objects;
    float iteration_time;
    float simulation_speed;
    bool mouse_lock;
    bool program_closed;
    sem_t* graphics_ready_sem;
} thread_info;

#define CONSTANT_K 8.999e9 // m^2/C^2

#endif // TYPES_H