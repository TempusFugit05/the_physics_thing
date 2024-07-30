#ifndef EQUATIONS_H
#define EQUATIONS_H
#include <stdbool.h>
#include "types.h"

double get_position(const double start_velocity, const double acceleration, const double time);
// double get_position(const double velocity, const double delta_time);
double get_velocity(const double acceleration, const double delta_time);
double get_kinetic_energy(const double velocity, const double mass);
double energy_to_velocity(const double energy, const double mass);
double get_work(const double force, const double direction_deg, const double distance);
double get_friction(const double normal_force, const double friction_coefficient, const bool direction_sign);
double get_second_law_acceleration(const double force, const double mass);
double get_collision_speed(const double vel_1, const double mass_1, const double vel_2, const double mass_2);
double get_potential_energy(const double height, const double mass, const double gravity);
double get_distance(const position_t pos_1, const position_t pos_2);
double get_electric_force(const double charge_1, const double charge_2, const double distance);
#endif // EQUATIONS_H