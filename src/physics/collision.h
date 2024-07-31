//
// Created by josue on 2024-07-29.
//

#ifndef PIKUMA_PHYSICS_COLLISION_H
#define PIKUMA_PHYSICS_COLLISION_H

#include "cglm/cglm.h"

struct Body;

typedef struct Contact {
    struct Body *a;
    struct Body *b;
    vec2 start;
    vec2 end;
    vec2 normal;
    float depth;
} Contact;

void circle_check_resolve_boundary(struct Body *body, const vec2 min, const vec2 max);
bool circle_circle_collision_check(struct Body *a, struct Body *b, Contact *contact);

void box_check_resolve_boundary(struct Body *body, const vec2 min, const vec2 max);

void resolve_collision(Contact contact);

#endif //PIKUMA_PHYSICS_COLLISION_H
