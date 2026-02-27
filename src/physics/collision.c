//
// Created by josue on 2024-07-29.
//

#include "collision.h"
#include <float.h>
#include "rigidbodies.h"
#include "raymath.h"

static bool bounding_square_check_boundary_offset(const BoundingSquare bs, const Vector2 min, const Vector2 max, Vector2 *offset) {
    bool collide = false;
    if (bs.min.x < min.x) { offset->x = min.x - bs.min.x; collide = true; }
    else if (bs.max.x > max.x) { offset->x = max.x - bs.max.x; collide = true; }
    if (bs.min.y < min.y) { offset->y = min.y - bs.min.y; collide = true; }
    else if (bs.max.y > max.y){ offset->y = max.y - bs.max.y; collide = true; }

    return collide;
}

float get_minimum_separation(const PolygonShape* a, const PolygonShape* b) {
    float separation = -(FLT_MAX);

    for (int i = 0; i < a->vertex_count; ++i) {
        Vector2 normal = Vector2Subtract(a->transformed_vertices[i % (a->vertex_count - 1)], a->transformed_vertices[i]);

        normal = (Vector2){normal.y, -normal.x}; // rotate 90 degrees
        float min_sep = FLT_MAX;
        for (int j = 0; j < b->vertex_count; ++j) {
            min_sep = fminf(min_sep, Vector2DotProduct(Vector2Subtract(b->transformed_vertices[j], a->transformed_vertices[i]), normal));
        }

        if (min_sep > separation) { separation = min_sep; }
    }

    return separation;
}

bool are_aabs_overlapping(const BoundingSquare a, const BoundingSquare b) {
    return !(a.min.x > b.max.x || a.max.x < b.min.x || a.min.y > b.max.y || a.max.y < b.min.y);
}

bool circle_circle_collision_check(Body *a, Body *b, Contact *contact) {
    const Vector2 ab = Vector2Subtract(b->position, a->position);
    const float radius_sum  = a->circle_shape.radius + b->circle_shape.radius;
    if (Vector2LengthSqr(ab) < radius_sum * radius_sum) {
        const Vector2 normal = Vector2Normalize(ab);
        *contact = (Contact) {
            .a = a,
            .b = b,
            .start = Vector2Add(a->position, Vector2Scale(normal, a->circle_shape.radius)),
            .end =  Vector2Subtract(b->position, Vector2Scale(normal, b->circle_shape.radius)),
            .normal = normal,
            .depth = a->circle_shape.radius + b->circle_shape.radius - Vector2Length(ab)
        };
        return true;
    }
    return false;
}

bool polygon_polygon_collision_check(Body* a, Body* b, Contact* contact) {
    if (get_minimum_separation(&a->polygon_shape, &b->polygon_shape) <= 0 && get_minimum_separation(&b->polygon_shape, &a->polygon_shape) <= 0) {
        *contact = (Contact) {
            .a = a,
            .b = b,
            .start = (Vector2){0},
            .end = (Vector2){0},
            .normal = (Vector2){0},
            .depth = 0
        };
        return true;
    }

    return false;
}

void resolve_collision(const Contact contact) {
    const float inverse_mass_sum = contact.a->inverse_mass + contact.b->inverse_mass;

    // resolve penetration
    const float depth_mass = contact.depth / inverse_mass_sum;
    const float depth_a = depth_mass * contact.b->inverse_mass;
    const float depth_b = depth_mass * contact.a->inverse_mass;
    contact.a->position = Vector2Subtract(contact.a->position, Vector2Scale(contact.normal, depth_a));
    contact.b->position = Vector2Add( contact.b->position, Vector2Scale(contact.normal, depth_b));

    // resolve collision
    const Vector2 relative_velocity = Vector2Subtract(contact.a->linear_velocity, contact.b->linear_velocity);

    const float e = fminf(contact.a->restitution, contact.b->restitution);
    const float impulse_magnitude = -(1 + e) * Vector2DotProduct(relative_velocity, contact.normal) / inverse_mass_sum;

    const Vector2 impulse_a = Vector2Scale(contact.normal, impulse_magnitude);
    body_apply_impulse(contact.a, impulse_a);
    const Vector2 impulse_b = Vector2Scale(contact.normal, -impulse_magnitude);
    body_apply_impulse(contact.b, impulse_b);
}

void check_resolve_boundary(Body* body, const Vector2 min, const Vector2 max) {
    Vector2 offset = {0};
    if (bounding_square_check_boundary_offset(body->bounding_square, min, max, &offset)) {
        body->position = Vector2Add(body->position, offset);
        body->linear_velocity = Vector2Multiply(body->linear_velocity, Vector2Scale(Vector2Normalize(offset), 0.75f));
    }
}
