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

float get_minimum_separation(const PolygonShape* a, const PolygonShape* b, Vector2 *out_normal, Vector2 *out_point) {
    float separation = -(FLT_MAX);

    for (int i = 0; i < a->vertex_count; ++i) {
        const Vector2 edge = Vector2Subtract(a->tfmd_vertices[(i + 1) % a->vertex_count], a->tfmd_vertices[i]);
        const Vector2 edge_n = Vector2Normalize(edge);
        const Vector2 normal = {edge_n.y, -edge_n.x};

        float min_sep = FLT_MAX;
        Vector2 point = {0};
        for (int j = 0; j < b->vertex_count; ++j) {
            const float sep = Vector2DotProduct(Vector2Subtract(b->tfmd_vertices[j], a->tfmd_vertices[i]), normal);
            if (sep < min_sep) {
                min_sep = sep;
                point = b->tfmd_vertices[j];
            }
        }

        if (min_sep > separation) {
            *out_normal = normal;
            *out_point = point;
            separation = min_sep;
        }
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
    Vector2 normal_ab, normal_ba;
    Vector2 start_ab, start_ba;
    const float min_sep_ab = get_minimum_separation(&a->polygon_shape, &b->polygon_shape, &normal_ab, &start_ab);
    const float min_sep_ba = get_minimum_separation(&b->polygon_shape, &a->polygon_shape, &normal_ba, &start_ba);

    if (min_sep_ab <= 0 && min_sep_ba <= 0) {
        if (min_sep_ab > min_sep_ba) {
            *contact = (Contact) {
                .a = a,
                .b = b,
                .start = start_ab,
                .end = Vector2Add(start_ab, Vector2Scale(normal_ab, min_sep_ab)),
                .normal = normal_ab,
                .depth = -min_sep_ab
            };
        } else {
            *contact = (Contact) {
                .a = a,
                .b = b,
                .start = start_ba,
                .end = Vector2Add(start_ba, Vector2Scale(normal_ba, min_sep_ba)),
                .normal = Vector2Negate(normal_ba),
                .depth = -min_sep_ba
            };
        }
        return true;
    }

    return false;
}

void resolve_penetration(const Contact contact) {
    if (contact.a->is_static && contact.b->is_static) { return; }

    const float inverse_mass_sum = contact.a->inverse_mass + contact.b->inverse_mass;
    const float depth_mass = contact.depth / inverse_mass_sum;
    const float depth_a = depth_mass * contact.a->inverse_mass;
    const float depth_b = depth_mass * contact.b->inverse_mass;

    contact.a->position = Vector2Subtract(contact.a->position, Vector2Scale(contact.normal, depth_a));
    contact.b->position = Vector2Add( contact.b->position, Vector2Scale(contact.normal, depth_b));
}

void resolve_collision(const Contact contact) {
    resolve_penetration(contact);

    // elasticity coefficient of restitution
    const float e = fminf(contact.a->restitution, contact.b->restitution);

    // radius contact to point a
    const Vector2 ra = Vector2Subtract(contact.end, contact.a->position);
    // radius contact to point b
    const Vector2 rb = Vector2Subtract(contact.start, contact.b->position);

    // relative velocity is (linear A + angular A) - (linear B + angular B) at point p
    Vector2 va = Vector2Add(contact.a->linear_velocity, (Vector2){
        -ra.y * contact.a->angular_velocity,
        ra.x * contact.a->angular_velocity
    });
    Vector2 vb = Vector2Add(contact.b->linear_velocity, (Vector2){
        -rb.y * contact.b->angular_velocity,
        rb.x * contact.b->angular_velocity
    });

    const Vector2 relative_velocity = Vector2Subtract(va, vb);

    // calculate impulse
    const float inverse_mass_sum = contact.a->inverse_mass + contact.b->inverse_mass;

    // 2D cross product (sort of) yield scalar
    // const float ran = ra.x * contact.normal.y - ra.y * contact.normal.x;
    // const float rbn = rb.x * contact.normal.y - rb.y * contact.normal.x;
    const float ran = contact.normal.x * ra.y - contact.normal.y * ra.x;
    const float rbn = contact.normal.x * rb.y - contact.normal.y * rb.x;
    const float impulse_magnitude = -(1 + e) * Vector2DotProduct(relative_velocity, contact.normal) / (inverse_mass_sum + ran / contact.a->inverse_angular_mass + rbn / contact.b->inverse_angular_mass);

    // apply impulse
    const Vector2 impulse_a = Vector2Scale(contact.normal, impulse_magnitude);
    body_apply_impulse(contact.a, impulse_a, ra);
    const Vector2 impulse_b = Vector2Scale(contact.normal, -impulse_magnitude);
    body_apply_impulse(contact.b, impulse_b, rb);
}

void check_resolve_boundary(Body* body, const Vector2 min, const Vector2 max) {
    Vector2 offset = {0};
    if (bounding_square_check_boundary_offset(body->bounding_square, min, max, &offset)) {
        body->position = Vector2Add(body->position, offset);
        body->linear_velocity = Vector2Multiply(body->linear_velocity, Vector2Scale(Vector2Normalize(offset), 0.75f));
    }
}
