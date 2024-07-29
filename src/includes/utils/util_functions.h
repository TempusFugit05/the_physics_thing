#ifndef UTIL_FUNCTIONS_H
#define UTIL_FUNCTIONS_H
#include "../physics/include/types.h"

void dump_obj_info(const object_t* obj, char* string, const unsigned int string_size);

int meters_to_pixel(double meters);
double pixels_to_meter(int pixels);

void start_iteration();
double end_iteration();

#endif // UTIL_FUNCTIONS_H