//
// Created by josue on 2024-07-29.
//

#include "collision.h"
#include "rigidbodies.h"
#include "raymath.h"

void circle_check_resolve_boundary(Body *body, const Vector2 min, const Vector2 max) {
    if (body->position.x -body->circle_shape.radius < min.x) {
        body->position.x = body->circle_shape.radius + min.x;
        body->linear_velocity.x = -body->linear_velocity.x * 0.75f;
    } else if (body->position.x +body->circle_shape.radius > max.x){
        body->position.x = max.x - body->circle_shape.radius;
        body->linear_velocity.x = -body->linear_velocity.x * 0.75f;
    }
    if (body->position.y -body->circle_shape.radius < min.y) {
        body->position.y = body->circle_shape.radius + min.y;
        body->linear_velocity.y = -body->linear_velocity.y * 0.75f;
    } else if (body->position.y + body->circle_shape.radius > max.y){
        body->position.y = max.y - body->circle_shape.radius;
        body->linear_velocity.y = -body->linear_velocity.y * 0.75f;
    }
}

bool circle_circle_collision_check(Body *a, Body *b, Contact *contact) {
    const Vector2 ab = Vector2Subtract(b->position, a->position);
    const float radius_sum  = a->circle_shape.radius + b->circle_shape.radius;
    if (Vector2LengthSqr(ab) < radius_sum * radius_sum) {
        contact->a = a;
        contact->b = b;

        contact->normal = Vector2Normalize(ab); // collision normal
        contact->depth = (a->circle_shape.radius + b->circle_shape.radius) - Vector2Length(ab);

        // start and end of the collision. Both points at the edge of each circle along the collision normal
        contact->start = Vector2Subtract(contact->start, Vector2Scale(contact->normal, b->circle_shape.radius));
        contact->start = Vector2Add(contact->start, b->position);
        contact->end = Vector2Add(contact->end, Vector2Scale(contact->normal, a->circle_shape.radius));
        contact->end = Vector2Add(contact->end, a->position);

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

void box_check_resolve_boundary(Body *body, const Vector2 min, const Vector2 max) {
    if (body->position.x - body->box_shape.center.x - body->box_shape.extents.x < min.x) {
        body->position.x = body->box_shape.center.x + body->box_shape.extents.x + min.x;
        body->linear_velocity.x = -body->linear_velocity.x * 0.75f;
    } else if (body->position.x - body->box_shape.center.x + body->box_shape.extents.x > max.x){
        body->position.x = max.x - body->box_shape.center.x - body->box_shape.extents.x;
        body->linear_velocity.x = -body->linear_velocity.x * 0.75f;
    }
    if (body->position.y - body->box_shape.center.y - body->box_shape.extents.y < min.y) {
        body->position.y = body->box_shape.center.y + body->box_shape.extents.y + min.y;
        body->linear_velocity.y = -body->linear_velocity.y * 0.75f;
    } else if (body->position.y - body->box_shape.center.y + body->box_shape.extents.y > max.y){
        body->position.y = max.y - body->box_shape.center.y - body->box_shape.extents.y;
        body->linear_velocity.y = -body->linear_velocity.y * 0.75f;
    }
}
