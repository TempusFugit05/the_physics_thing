#include "../include/equations.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

inline double get_velocity(const double acceleration, const double delta_time)
{
    if (acceleration != 0)
    {
        double vel = acceleration * delta_time;
        return (isnan(vel) ? 0 : vel);
    }

    return 0;
}

inline double get_position(const double start_velocity, const double acceleration, const double time)
{
    return (start_velocity * time) + ((acceleration * time * time) / 2);
}

// inline double get_position(const double velocity, const double delta_time)
// {
//     if (velocity != 0)
//     {
//         double position = velocity * delta_time;
//         return (isnan(position) ? 0 : position);
//     }
//     return 0;
// }

inline double get_kinetic_energy(const double velocity, const double mass)
{
    if (velocity != 0)
    {
        double energy = mass * velocity * velocity/2;
        return (isnan(energy) ? 0 : energy);
    }
    return 0;
}

inline double energy_to_velocity(const double energy, const double mass)
{
    if (energy != 0)
    {
        double vel = pow(2 * energy / mass, 0.5);
        return (isnan(vel) ? 0 : vel);
    }
    return 0;
}

inline double get_work(const double force, const double direction_deg, const double distance)
{
    double work = force * cos(direction_deg) * distance;
    return (isnan(work)) ? 0 : work;
}

inline double get_friction(const double normal_force, const double friction_coefficient, const bool direction_sign)
{
    double force = normal_force * friction_coefficient * ((direction_sign) ? 1 : -1);
    return (isnan(force)) ? 0 : force;
}

inline double get_second_law_acceleration(const double force, const double mass)
{
    if (force == 0)
    {
        return 0;
    }
    
    return mass / force;
}

inline double get_collision_speed(const double vel_1, const double mass_1, const double vel_2, const double mass_2)
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

inline double get_potential_energy(const double height, const double mass, const double gravity)
{
    return height * mass * fabs(gravity);
}