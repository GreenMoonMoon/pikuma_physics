//
// Created by josue on 2024-07-29.
//

#ifndef PIKUMA_PHYSICS_COLLISION_H
#define PIKUMA_PHYSICS_COLLISION_H

#include "raylib.h"
#include "physic_types.h"

struct Body;

typedef struct Contact {
    struct Body *a;
    struct Body *b;
    Vector2 start;
    Vector2 end;
    Vector2 normal;
    float depth;
} Contact;

float get_minimum_separation(const PolygonShape* a, const PolygonShape* b, Vector2 *out_normal, Vector2 *out_point);

bool are_aabs_overlapping(BoundingSquare a, BoundingSquare b);

bool circle_circle_collision_check(struct Body *a, struct Body *b, Contact *contact);

bool polygon_polygon_collision_check(struct Body *a, struct Body *b, Contact *contact);

void check_resolve_boundary(struct Body *body, Vector2 min, Vector2 max);

void resolve_collision(Contact contact);

#endif //PIKUMA_PHYSICS_COLLISION_H
