#include "../include/equations.h"
#include "../include/types.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

inline psize_t get_velocity(const psize_t acceleration, const psize_t delta_time)
{
    if (acceleration != 0)
    {
        psize_t vel = acceleration * delta_time;
        return (isnan(vel) ? 0 : vel);
    }

    return 0;
}

inline psize_t get_position(const psize_t start_velocity, const psize_t acceleration, const psize_t time)
{
    return (start_velocity * time) + ((acceleration * time * time) / 2);
}

// inline psize_t get_position(const psize_t velocity, const psize_t delta_time)
// {
//     if (velocity != 0)
//     {
//         psize_t position = velocity * delta_time;
//         return (isnan(position) ? 0 : position);
//     }
//     return 0;
// }

inline psize_t get_kinetic_energy(const psize_t velocity, const psize_t mass)
{
    if (velocity != 0)
    {
        psize_t energy = mass * velocity * velocity/2;
        return (isnan(energy) ? 0 : energy);
    }
    return 0;
}

inline psize_t energy_to_velocity(const psize_t energy, const psize_t mass)
{
    if (energy != 0)
    {
        psize_t vel = pow(2 * energy / mass, 0.5);
        return (isnan(vel) ? 0 : vel);
    }
    return 0;
}

inline psize_t get_work(const psize_t force, const psize_t direction_deg, const psize_t distance)
{
    psize_t work = force * cos(direction_deg) * distance;
    return (isnan(work)) ? 0 : work;
}

inline psize_t get_friction(const psize_t normal_force, const psize_t friction_coefficient, const bool direction_sign)
{
    psize_t force = normal_force * friction_coefficient * ((direction_sign) ? 1 : -1);
    return (isnan(force)) ? 0 : force;
}

inline psize_t get_second_law_acceleration(const psize_t force, const psize_t mass)
{
    if (force == 0 || mass == INFINITY)
    {
        return 0;
    }
    
    return force / mass;
}

inline psize_t get_collision_speed(const psize_t vel_1, const psize_t mass_1, const psize_t vel_2, const psize_t mass_2)
{
    /*Takes two objects and returns the speed of the first object after a collision has occured*/
    if (mass_1 == INFINITY)
    {
        return vel_1;
    }

    else if(mass_2 == INFINITY)
    {
        return -vel_1;
    }
    
    return (((mass_1 - mass_2) / (mass_1 + mass_2)) * vel_1) + (((2 * mass_2) / (mass_1 + mass_2)) * vel_2);
}

inline psize_t get_potential_energy(const psize_t height, const psize_t mass, const psize_t gravity)
{
    return height * mass * fabs(gravity);
}

inline psize_t get_distance(const vector_2 pos_1, const vector_2 pos_2)
{
    psize_t delta_x = pos_1.x - pos_2.x;
    psize_t delta_y = pos_1.y - pos_2.y;
    return sqrt(delta_x*delta_x + delta_y*delta_y);
}

inline psize_t get_electric_force(const psize_t charge_1, const psize_t charge_2, const psize_t distance)
{
    return (CONSTANT_K * charge_1 * charge_2) / (distance * distance);
}