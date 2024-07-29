#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include "../physics/include/types.h"

object_t* get_object(registry_t* registry, unsigned int index);
void create_object(registry_t* registry, object_t* object_params);

#endif // OBJECT_MANAGER_H