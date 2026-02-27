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

static bool enable_gravity = true;
enum Mode {
    MODE_NONE,
    MODE_ADD_SHAPE,
    MODE_DRAG_DEBUG_WINDOW,
};
enum AddMode {
    ADD_MODE_CIRCLE,
    ADD_MODE_BOX,
    ADD_MODE_POLYGON,

    ADD_MODE_COUNT
};

struct SpawnInfo {
    Vector2 position;
    float mass;
    bool setup;
    bool set;
    union {
        PolygonShape polygon_info;
        CircleShape circle_info;
        BoxShape box_info;
    };
};
static enum Mode mode = MODE_NONE;
static enum AddMode add_mode =  ADD_MODE_CIRCLE;
static struct SpawnInfo spawn_info = {
        .position = {0},
        .mass = 1.0f,
        .setup = false,
        .set = false,
        .circle_info = (CircleShape){
            .radius = PIXEL_PER_UNIT
        }
};

static Vector2 vertex_buffer[3] = {
    {-100.0f, 100.0f},
    {0.0f, -100.0f},
    {100.0f, 100.0f}
};
static Vector2 spawn_polygon_vertices[3] = {
    {-PIXEL_PER_UNIT, PIXEL_PER_UNIT},
    {0, -PIXEL_PER_UNIT},
    {PIXEL_PER_UNIT, PIXEL_PER_UNIT}
};

static Texture2D background;

static void handle_inputs(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { mode = MODE_NONE; }
    if (IsKeyPressed(KEY_N)) {
        if (mode == MODE_NONE) {
            mode = MODE_ADD_SHAPE;
            add_mode = ADD_MODE_CIRCLE;

            // initialize the spawn information
            spawn_info = (struct SpawnInfo) {
                .position = (Vector2){0},
                .mass = 1.0f,
                .setup = false,
                .set = false,
                .circle_info = (CircleShape) { .radius = PIXEL_PER_UNIT }
            };
        } else if (mode == MODE_ADD_SHAPE) {
            // loop over add mode shapes
            add_mode = (add_mode + 1) % ADD_MODE_COUNT;

            // initialize spawn info each time the tool change shapes
            switch (add_mode) {
            case ADD_MODE_CIRCLE:
                spawn_info.circle_info = (CircleShape) {
                    .radius = PIXEL_PER_UNIT
                };
                break;
            case ADD_MODE_BOX:
                spawn_info.box_info  = (BoxShape) {
                    .center = (Vector2){0},
                    .extents = (Vector2){(float)PIXEL_PER_UNIT / 2, (float)PIXEL_PER_UNIT / 2}
                };
                break;
            case ADD_MODE_POLYGON:
                spawn_info.polygon_info = (PolygonShape) {
                    .vertices = NULL,
                    .vertex_count = 3
                };
                arrput(spawn_info.polygon_info.vertices, spawn_polygon_vertices[0]);
                arrput(spawn_info.polygon_info.vertices, spawn_polygon_vertices[1]);
                arrput(spawn_info.polygon_info.vertices, spawn_polygon_vertices[2]);
                break;
            default: break;
            }
        }
    }
    if (IsKeyPressed(KEY_PAUSE)) { paused = !paused; }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        step = 1;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        step = -1; // TODO: implement a caching system ?
    }

    // Check inputs based on mode
    switch (mode) {
    case MODE_ADD_SHAPE:
        switch (add_mode) {
        case ADD_MODE_CIRCLE:
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (spawn_info.setup) {
                    spawn_info.circle_info.radius = Clamp(fabsf((float)GetMouseX() - spawn_info.position.x), 1.0f * PIXEL_PER_UNIT, 100.0f * PIXEL_PER_UNIT);
                    spawn_info.mass = Clamp(fabsf((float)GetMouseY() - spawn_info.position.y) * 0.1f, 1.0f, 100.0f);
                } else {
                    spawn_info.position = GetMousePosition();
                    spawn_info.setup = true;
                }
            }
            if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                if (spawn_info.setup && !spawn_info.set) {
                    spawn_info.set = true;
                }
            }
            if (IsKeyDown(KEY_ENTER)) {
                arrput(bodies, create_circle_body(spawn_info.circle_info.radius, spawn_info.mass, 0.9f, spawn_info.position));
                mode = MODE_NONE;
            }
            break;
        case ADD_MODE_BOX:
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                if (spawn_info.setup) {
                    spawn_info.box_info.extents = Vector2Clamp(
                        (Vector2){
                            fabsf((float)GetMouseX() - spawn_info.position.x),
                            fabsf((float)GetMouseY() - spawn_info.position.y)
                        },
                        (Vector2){PIXEL_PER_UNIT, PIXEL_PER_UNIT},
                        (Vector2){100.0f * PIXEL_PER_UNIT, 100.0f * PIXEL_PER_UNIT}
                    );
                } else {
                    spawn_info.position = GetMousePosition();
                    spawn_info.setup = true;
                }
            }
            if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                if (spawn_info.setup && !spawn_info.set) {
                    spawn_info.set = true;
                }
            }
            if (IsKeyDown(KEY_ENTER)) {
                arrput(bodies, create_box_body(spawn_info.box_info.center, spawn_info.box_info.extents, spawn_info.mass, 0.9f, spawn_info.position));
                mode = MODE_NONE;
            }
            break;
        case ADD_MODE_POLYGON:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                spawn_info.position = GetMousePosition();
                spawn_info.setup = true;
            }
            if (IsKeyDown(KEY_ENTER)) {
                arrput(bodies, create_polygon_body(spawn_info.polygon_info.vertices, spawn_info.polygon_info.vertex_count, spawn_info.mass, 0.9f, spawn_info.position));
                mode = MODE_NONE;
            }
            break;
        default: break;
        }
        break;
    case MODE_DRAG_DEBUG_WINDOW:
        break;
    default: break;
    }
}

void rigidbodies_scene_init(void) {
    background = LoadTexture("../assets/PNG/Backgrounds/blue_grass.png");

    // arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, 0.9f, (Vector2){300, 300}));
    // arrput(bodies, create_circle_body(2.0f * PIXEL_PER_UNIT, 2.0f, 0.9f, (Vector2){325, 100}));

    // arrput(bodies, create_box_body((Vector2){0}, (Vector2){50,50}, 2.0f, 0.9f, (Vector2){700, 400}));
    // arrput(bodies, create_box_body((Vector2){0}, (Vector2){50,50}, 2.0f, 0.5f, (Vector2){650, 200}));
    // bodies[0].rotation = 0.1f;

    // arrput(bodies, create_polygon_body(vertex_buffer, 3, 1.0f, 0.5f, (Vector2){200, 200}));
    arrput(bodies, create_polygon_body(vertex_buffer, 3, 1.0f, 0.5f, (Vector2){450, 50}));
    arrput(bodies, create_polygon_body(vertex_buffer, 3, 1.0f, 0.5f, (Vector2){300, 230}));

    bodies[1].angular_velocity = -0.2f;
    // bodies[3].angular_velocity = 0.1f;
    // bodies[4].angular_velocity = -0.1f;
}

void rigidbodies_scene_update(const float delta_time) {
    // INPUTS
    handle_inputs();

    // Handle spawning new bodies
    if(mode == MODE_ADD_SHAPE && !spawn_info.setup){
        spawn_info.position = GetMousePosition();
    }

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
        Vector2 forces = {0.0f, 0.0f};

        if (enable_gravity) { forces.y = 10.0f * PIXEL_PER_UNIT; } // add gravity;
        force_apply_drag(body->linear_velocity, 0.001f, &forces);

        // Integrate  forces
        body_integrate_linear(body, forces, delta_time);

        // add torques
        float torques = 0;

        // integrate torques
        body_integrate_angular(body, torques, delta_time);

        // check boundary collisions
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
        check_resolve_boundary(&bodies[i], (Vector2){0}, (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()});
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
        // resolve_collision(collisions[i]);
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

    if(mode == MODE_ADD_SHAPE){
        switch (add_mode) {
        case ADD_MODE_CIRCLE:
            DrawCircleLines(spawn_info.position.x, spawn_info.position.y, spawn_info.circle_info.radius, DARKGRAY);
            break;
        case ADD_MODE_BOX:
            DrawRectangleLines(
                spawn_info.position.x - spawn_info.box_info.extents.x,
                spawn_info.position.y - spawn_info.box_info.extents.y,
                spawn_info.box_info.extents.x * 2.0f,
                spawn_info.box_info.extents.y * 2.0f,
                DARKGRAY
            );
            break;
        case ADD_MODE_POLYGON:
            draw_polygon(spawn_info.polygon_info.tfmd_vertices, spawn_info.polygon_info.vertex_count, DARKGRAY);
            break;
        default: break;
        }
        DrawText(TextFormat("%.1f", spawn_info.mass), spawn_info.position.x, spawn_info.position.y, 18, DARKGRAY);
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
        draw_collision(collisions[i].start, collisions[i].normal, collisions[i].depth, ORANGE);
    }

    // UI
    DrawText(TextFormat("Collision count: %d", arrlen(collisions)), 10, 10, 20, DARKGREEN);
    DrawText("Add new shape (N)", 10, 35, 20, BLACK);
    DrawText("Pause (Pause)", 10, 60, 20, BLACK);
    DrawText("Step (Right arrow)", 10, 85, 20, BLACK);
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
