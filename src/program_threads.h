#ifndef PROGRAM_THREADS_H
#define PROGRAM_THREADS_H

void* physics_thread(void* args);
void* graphics_thread(void* args);

#endif // PROGRAM_THREADS_H