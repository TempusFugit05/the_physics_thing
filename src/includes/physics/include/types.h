#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <semaphore.h>
#include "configs.h"

typedef struct
{
    psize_t x;
    psize_t y;
} vector_2;

typedef struct object_t
{
    vector_2 position;
    vector_2 velocity;
    vector_2 acceleration;
    vector_2 force;
    psize_t mass;
    psize_t size;
    Color color;
    psize_t charge;
    char name[32];
    volatile bool is_target;
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
    
    volatile float simulation_time;
    volatile unsigned long int iteration_num;
    volatile float simulation_speed;

    volatile bool program_closed;
    volatile bool is_paused;

    sem_t* graphics_ready_sem;
    sem_t* request_objects_sem;
} thread_info;

#define CONSTANT_K 8.999e9 // m^2/C^2

#endif // TYPES_H