//
// Created by josue on 2024-07-29.
//

#ifndef PIKUMA_PHYSICS_FORCES_H
#define PIKUMA_PHYSICS_FORCES_H

#include "raylib.h"

void force_add_force(Vector2 force, float inverse_mass, Vector2 *out_forces);
void force_apply_drag(Vector2 velocity, float coefficient, Vector2 *out_forces);

#endif //PIKUMA_PHYSICS_FORCES_H
