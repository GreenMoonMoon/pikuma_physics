//
// Created by josue on 2024-07-22.
//

#include "raylib.h"
#include "raymath.h"
#include "rigidbodies_scene.h"
#include "../physics/rigidbodies.h"
#include "../physics/collision.h"
#include "../physics/forces.h"
#include "../extern/stb_ds.h"
#include "../utils/debug_draw.h"

static Body *bodies = nullptr;
static Contact *collisions = nullptr;

static bool paused = false;
static int step = 0;

static Vector2 wind_input = {0};

static bool enable_gravity = true;

static Texture2D background;

static void handle_inputs(void) {
    if (paused) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) { step++; }
    } else {
        wind_input = (Vector2){
            IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT),
            IsKeyDown(KEY_UP) - IsKeyDown(KEY_DOWN)
        };
    }
    if (IsKeyPressed(KEY_PAUSE)) { paused = !paused; }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        arrput(bodies, create_box_body((Vector2){0}, (Vector2){50, 50}, 1.0f, 0.5f, GetMousePosition(), false));
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        arrput(bodies, create_circle_body(50.0f, 1.0f, 0.5f, GetMousePosition()));
    }
}

void rigidbodies_scene_init(void) {
    background = LoadTexture("../assets/PNG/Backgrounds/blue_grass.png");

    arrput(bodies, create_box_body((Vector2){0}, (Vector2){GetScreenWidth(), 20}, 0, 0.2f, (Vector2){0, GetScreenHeight() - 20}, true));
    arrput(bodies, create_box_body((Vector2){0}, (Vector2){50,50}, 0, 0.9f, (Vector2){700, 400}, true));
    bodies[1].rotation = 0.1f;
}

void rigidbodies_scene_update(const float delta_time) {
    // INPUTS
    handle_inputs();

    // Handle pause and step
    if (paused) {
        if (step == 0) { return; }
        step = 0;
    }

    // clear collisions
    arrsetlen(collisions, 0);

    // Apply forces and check boundary collisions
    for (int i = 0; i < arrlen(bodies); ++i) {
        Body *body = &bodies[i];

        // add forces
        Vector2 forces = Vector2Scale(wind_input, 100.0f);

        if (enable_gravity) {
            forces.y = 10.0f * PIXEL_PER_UNIT;
        } // add gravity;
        force_apply_drag(body->linear_velocity, 0.001f, &forces);

        // Integrate  forces
        if (!body->is_static) { body_integrate_linear(body, forces, delta_time); }

        // add torques
        float torques = 0;

        // integrate torques
        body_integrate_angular(body, torques, delta_time);

        // update bounding square
        switch (bodies[i].type) {
        case BOX_SHAPE_TYPE:
            case POLYGON_SHAPE_TYPE:
                body->bounding_square = get_polygon_bounding_square(body->polygon_shape, body->position, body->rotation);
                break;
            case CIRCLE_SHAPE_TYPE:
                body->bounding_square = (BoundingSquare) {
                    .min = Vector2Subtract(body->position, (Vector2){body->circle_shape.radius, body->circle_shape.radius}),
                    .max = Vector2Add(body->position, (Vector2){body->circle_shape.radius, body->circle_shape.radius}),
                };
                break;
        }

        // check boundary collisions
        // check_resolve_boundary(&bodies[i], (Vector2){0}, (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()});
        if (bodies[i].bounding_square.min.y - 100 > GetScreenHeight()) { // the 100 is a small buffer
            arrdelswap(bodies, i);
        }
    }

    // Check for collisions
    // Loop over each bodies pair and check for collisions
    for (int i = 0; i < arrlen(bodies) - 1; ++i) {
        for (int j = i + 1; j < arrlen(bodies); ++j) {
            Contact contact = {nullptr};

            // check aabs overlap before checking more detailed collisions
            if (are_aabs_overlapping(bodies[i].bounding_square, bodies[j].bounding_square)) {
                bodies[i].aabs_is_overlapping = true;
                bodies[j].aabs_is_overlapping = true;

                switch (bodies[i].type) {
                case BOX_SHAPE_TYPE:
                case POLYGON_SHAPE_TYPE:
                    switch (bodies[j].type) {
                    case BOX_SHAPE_TYPE:
                    case POLYGON_SHAPE_TYPE:
                        if (polygon_polygon_collision_check(&bodies[i], &bodies[j], &contact)) {
                            bodies[i].is_colliding = true;
                            bodies[j].is_colliding = true;
                            arrput(collisions, contact);
                        }
                        break;
                    case CIRCLE_SHAPE_TYPE:
                        break;
                    }
                    break;
                case CIRCLE_SHAPE_TYPE:
                    switch (bodies[j].type) {
                    case BOX_SHAPE_TYPE:
                    case POLYGON_SHAPE_TYPE:
                        break;
                    case CIRCLE_SHAPE_TYPE:
                        if (circle_circle_collision_check(&bodies[i], &bodies[j], &contact)) {
                            bodies[i].is_colliding = true;
                            bodies[j].is_colliding = true;
                            arrput(collisions, contact);
                        }
                        break;
                    }
                    break;
                }

            }
        }
    }

    // resolve previous frame contact
    for (int i = 0; i < arrlen(collisions); ++i) {
        resolve_collision(collisions[i]);
    }

    // commented out for now and cleared at the beginning of the frame to allow debug draw
    // arrsetlen(debug_collisions, 0);
}

void rigidbodies_scene_render(void) {
    DrawTexture(background, 0, -100, WHITE);

    for (int i = 0; i < arrlen(bodies); ++i) {
        const Color color = bodies[i].is_colliding ? RED : BLACK;
        switch (bodies[i].type) {
        case BOX_SHAPE_TYPE:
        case POLYGON_SHAPE_TYPE:
            draw_polygon(bodies[i].polygon_shape.tfmd_vertices, bodies[i].polygon_shape.vertex_count, color);
            break;
        case CIRCLE_SHAPE_TYPE:
            draw_circle_shape(bodies[i].position, bodies[i].circle_shape.radius, bodies[i].rotation, color);
            break;
        }
        bodies[i].is_colliding = false; // clear flag for next frame
    }

    // DEBUG
    // draw aabs
    for (int i = 0; i < arrlen(bodies); ++i) {
        const Color color = bodies[i].aabs_is_overlapping ? ORANGE : BLUE;
        draw_aabs(bodies[i].bounding_square.min, bodies[i].bounding_square.max, color);
        bodies[i].aabs_is_overlapping = false; // clear flag
    }
    // draw collisions
    for (int i = 0; i < arrlen(collisions); ++i) {
        DrawRectangle(collisions[i].start.x - 3, collisions[i].start.y - 3, 6, 6, RED);
        DrawLineV(collisions[i].start, collisions[i].end, BLUE);
    }

    // UI
    DrawText(TextFormat("Body count: %d", arrlen(bodies)), 10, 10, 20, DARKGREEN);
    DrawText(TextFormat("Collision count: %d", arrlen(collisions)), 10, 40, 20, DARKGREEN);
    DrawText("Pause (Pause)", 10, 85, 20, BLACK);
    DrawText("Step (Right arrow)", 10, 120, 20, BLACK);
}

void rigidbodies_scene_cleanup(void) {
    for (int i = 0; i < arrlen(bodies); ++i) {
        if (bodies[i].type == POLYGON_SHAPE_TYPE) { free_polygon_shape(bodies[i].polygon_shape); }
    }
    arrfree(bodies);
    arrfree(collisions);
    UnloadTexture(background);
}

void rigidbodies_scene_load(PhysicScene *scene) {
    scene->init = rigidbodies_scene_init;
    scene->update = rigidbodies_scene_update;
    scene->render = rigidbodies_scene_render;
    scene->cleanup = rigidbodies_scene_cleanup;
}
