#ifndef EQUATIONS_H
#define EQUATIONS_H
#include <stdbool.h>
#include "types.h"
#include "configs.h"

psize_t get_position(const psize_t start_velocity, const psize_t acceleration, const psize_t time);
// psize_t get_position(const psize_t velocity, const psize_t delta_time);
psize_t get_velocity(const psize_t acceleration, const psize_t delta_time);
psize_t get_kinetic_energy(const psize_t velocity, const psize_t mass);
psize_t energy_to_velocity(const psize_t energy, const psize_t mass);
psize_t get_work(const psize_t force, const psize_t direction_deg, const psize_t distance);
psize_t get_friction(const psize_t normal_force, const psize_t friction_coefficient, const bool direction_sign);
psize_t get_second_law_acceleration(const psize_t force, const psize_t mass);
psize_t get_collision_speed(const psize_t vel_1, const psize_t mass_1, const psize_t vel_2, const psize_t mass_2);
psize_t get_potential_energy(const psize_t height, const psize_t mass, const psize_t gravity);
psize_t get_distance(const vector_2 pos_1, const vector_2 pos_2);
psize_t get_electric_force(const psize_t charge_1, const psize_t charge_2, const psize_t distance);
#endif // EQUATIONS_H