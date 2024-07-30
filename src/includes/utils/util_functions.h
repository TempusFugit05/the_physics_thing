#ifndef UTIL_FUNCTIONS_H
#define UTIL_FUNCTIONS_H
#include "../physics/include/types.h"

void dump_obj_info(const object_t* obj, char* string, const unsigned int string_size);

int meters_to_pixel(const double meters, const double pixels_per_meter);
double pixels_to_meter(int pixels, const double pixels_per_meter);

void start_iteration();
double end_iteration(float simulation_speed);

#endif // UTIL_FUNCTIONS_H