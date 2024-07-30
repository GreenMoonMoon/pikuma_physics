//
// Created by josue on 2024-07-29.
//

#ifndef PIKUMA_PHYSICS_FORCES_H
#define PIKUMA_PHYSICS_FORCES_H
#include "cglm/cglm.h"


void add_force(const vec2 force, float inverse_mass, vec2 out_forces);
void apply_drag_force(const vec2 velocity, float coefficient, vec2 out_forces);

#endif //PIKUMA_PHYSICS_FORCES_H
