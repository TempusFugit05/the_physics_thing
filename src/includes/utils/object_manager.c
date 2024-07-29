#include <stdlib.h>
#include <assert.h>

#include "../physics/include/types.h"
#include "./object_manager.h"

object_t* get_object(registry_t* registry, unsigned int index)
{
    if (index > registry->num_members)
    {
        return NULL;
    }

    member_t* current_member = registry->head;

    for (unsigned int i = 0; i < index; i++)
    {
        current_member = current_member->next_member;
        assert(current_member != NULL); // Check that the member is valid
    }
    
    return &current_member->obj;
}

void create_object(registry_t* registry, object_t* object_params)
{
    /*Create new list member*/
    member_t* new_member = malloc(sizeof(member_t));
    new_member->obj = *object_params;
    new_member->next_member = NULL;


    /*Iterate until the end of the list and add member*/
    if (registry->head != NULL)
    {
        member_t* current_member = registry->head;

        while (current_member->next_member != NULL)
        {
            current_member = current_member->next_member;
        }

        current_member->next_member = new_member;
    }

    else
    {
        registry->head = new_member;
    }

    registry->num_members++;
}