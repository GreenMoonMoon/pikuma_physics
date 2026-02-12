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
    MODE_ADD_CIRCLE,
    MODE_DRAG_DEBUG_WINDOW,
};
struct SpawnInfo {
    Vector2 position;
    float radius;
    float mass;
    bool set;
};
static enum Mode mode = MODE_NONE;
static struct SpawnInfo spawn_info = {
        .position = {0},
        .radius = PIXEL_PER_UNIT,
        .mass = 1.0f,
        .set = false,
};

static Vector2 *vertex_buffer = NULL;

static Texture2D background;
// static Texture2D sphere_texture;
// static Texture2D square_texture;

static void handle_inputs(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { mode = MODE_NONE; }
    if (IsKeyPressed(KEY_N)) { mode = MODE_ADD_CIRCLE; }
    if (IsKeyPressed(KEY_PAUSE)) { paused = !paused; }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        step = 1;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        step = -1; // TODO: implement a caching system ?
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        switch(mode){
            case MODE_NONE:
                break;
            case MODE_ADD_CIRCLE:
                spawn_info.position = GetMousePosition();
                spawn_info.set = true;
                break;
            default:
                break;
        }
    }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
        switch(mode){
            case MODE_ADD_CIRCLE:
                spawn_info.radius = Clamp(fabsf((float)GetMouseX() - spawn_info.position.x), 1.0f * PIXEL_PER_UNIT, 100.0f * PIXEL_PER_UNIT);
                spawn_info.mass = Clamp(fabsf((float)GetMouseY() - spawn_info.position.y) * 0.1f, 1.0f, 100.0f);
                break;
            default:
                break;
        }
    }else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        switch (mode) {
            case MODE_ADD_CIRCLE:
                arrput(bodies, create_circle_body(spawn_info.radius, spawn_info.mass, 0.9f, spawn_info.position));
                spawn_info.position = (Vector2){0};
                spawn_info.mass = 1.0f;
                spawn_info.radius = PIXEL_PER_UNIT;
                spawn_info.set = false;
                mode = MODE_NONE;
                break;
            case MODE_DRAG_DEBUG_WINDOW:
                mode = MODE_NONE;
                break;
            default:
                break;
        }
    }
}

void rigidbodies_scene_init(void) {
    background = LoadTexture("../assets/PNG/Backgrounds/blue_grass.png");

    // arrput(bodies, create_circle_body(1.0f * PIXEL_PER_UNIT, 1.0f, 0.9f, (Vector2){300, 300}));
    // arrput(bodies, create_circle_body(2.0f * PIXEL_PER_UNIT, 2.0f, 0.9f, (Vector2){325, 100}));

    // arrput(bodies, create_box_body((Vector2){0}, (Vector2){50,50}, 2.0f, 0.9f, (Vector2){700, 400}));
    // arrput(bodies, create_box_body((Vector2){0}, (Vector2){50,50}, 2.0f, 0.5f, (Vector2){650, 200}));

    vertex_buffer = malloc(sizeof(Vector2) * 3);
    vertex_buffer[0] = (Vector2){-100.0f, 100.0f};
    vertex_buffer[1] = (Vector2){0.0f, -100.0f};
    vertex_buffer[2] = (Vector2){100.0f, 100.0f};
    arrput(bodies, create_polygon_body(vertex_buffer, 3, 1.0f, 0.5f, (Vector2){200, 200}));

    arrput(bodies, create_polygon_body(vertex_buffer, 3, 1.0f, 0.5f, (Vector2){600, 300}));

    arrput(bodies, create_polygon_body(vertex_buffer, 3, 1.0f, 0.5f, (Vector2){300, 230}));
}

void rigidbodies_scene_update(const float delta_time) {
    // INPUTS
    handle_inputs();

    // Handle spawning new bodies
    if(mode == MODE_ADD_CIRCLE && !spawn_info.set){
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
        // add forces
        Vector2 forces = {0.0f, 0.0f};

        if (enable_gravity) { forces.y = 10.0f * PIXEL_PER_UNIT; } // add gravity;
        force_apply_drag(bodies[i].linear_velocity, 0.001f, &forces);

        // Integrate  forces
        body_integrate_linear(&bodies[i], forces, delta_time);


        // add torques
        float torques = 0.0f;

        // integrate torques
        body_integrate_angular(&bodies[i], torques, delta_time);

        // check boundary collisions
        Contact contact;
        switch (bodies[i].type) {
            case BOX_SHAPE_TYPE:
                box_check_resolve_boundary(&bodies[i], (Vector2){0}, (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()});
                break;
            case POLYGON_SHAPE_TYPE:
                break;
            case CIRCLE_SHAPE_TYPE:
                circle_check_resolve_boundary(&bodies[i], (Vector2){0}, (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()});
                break;
        }
    }

    // Check for collisions
    for (int i = 0; i < arrlen(bodies) - 1; ++i) {
        for (int j = i + 1; j < arrlen(bodies); ++j) {
            Contact contact = {nullptr};
            switch (bodies[i].type) {
                case BOX_SHAPE_TYPE:
                    switch (bodies[j].type) {
                        case BOX_SHAPE_TYPE:

                            break;
                        case POLYGON_SHAPE_TYPE:
                            break;
                        case CIRCLE_SHAPE_TYPE:
                            break;
                    }
                    break;
                case POLYGON_SHAPE_TYPE:
                    switch (bodies[j].type) {
                        case BOX_SHAPE_TYPE:
                                break;
                        case POLYGON_SHAPE_TYPE:
                                break;
                        case CIRCLE_SHAPE_TYPE:
                                break;
                    }
                    break;
                case CIRCLE_SHAPE_TYPE:
                    switch (bodies[j].type) {
                        case BOX_SHAPE_TYPE:
                                break;
                        case POLYGON_SHAPE_TYPE:
                                break;
                        case CIRCLE_SHAPE_TYPE:
                            if (circle_circle_collision_check(&bodies[i], &bodies[j], &contact)) { arrput(collisions, contact); } break;
                    }
                    break;
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
        switch (bodies[i].type) {
            case BOX_SHAPE_TYPE:
                DrawRectangleLines(bodies[i].position.x - bodies[i].box_shape.extents.x, bodies[i].position.y - bodies[i].box_shape.extents.y, 2 * bodies[i].box_shape.extents.x, 2 * bodies[i].box_shape.extents.y, BLACK);
                break;
            case POLYGON_SHAPE_TYPE:
                draw_polygon(bodies[i].position, bodies[i].polygon_shape.vertices, bodies[i].polygon_shape.vertex_count, BLACK);
                // DrawLineStrip(bodies[i].polygon_shape.vertices, bodies[i].polygon_shape.vertex_count, BLACK);
                break;
            case CIRCLE_SHAPE_TYPE:
                draw_circle_shape(bodies[i].position, bodies[i].circle_shape.radius, bodies[i].rotation, BLACK);
                break;
        }
    }

    if(mode == MODE_ADD_CIRCLE){
        DrawCircleLines(spawn_info.position.x, spawn_info.position.y, spawn_info.radius, BLACK);
        DrawText(TextFormat("%.1f", spawn_info.mass), spawn_info.position.x, spawn_info.position.y, 18, GREEN);
    }

    // DEBUG
    for (int i = 0; i < arrlen(collisions); ++i) {
        draw_collision(collisions[i].start, collisions[i].normal, ORANGE);
    }

    // UI
    DrawText(TextFormat("Collision count: %d", arrlen(collisions)), 10, 10, 20, DARKGREEN);
    DrawText("Add circle (N)", 10, 35, 20, BLACK);
    DrawText("Pause (Pause)", 10, 60, 20, BLACK);
    DrawText("Step (Right arrow)", 10, 85, 20, BLACK);

    // DEBUG
    for (int i = 0; i < arrlen(bodies) - 1; ++i) {
        for (int j = i + 1; j < arrlen(bodies); ++j) {
            const BoundingSquare a = {
                .min = Vector2Add(bodies[i].position, Vector2Subtract(bodies[i].box_shape.center,  bodies[i].box_shape.extents)),
                .max = Vector2Add(bodies[i].position, Vector2Add(bodies[i].box_shape.center,  bodies[i].box_shape.extents)),
            };
            const BoundingSquare b = {
                .min = Vector2Add(bodies[j].position, Vector2Subtract(bodies[j].box_shape.center,  bodies[j].box_shape.extents)),
                .max = Vector2Add(bodies[j].position, Vector2Add(bodies[j].box_shape.center,  bodies[j].box_shape.extents)),
            };
            if (is_aabb_aabb_overlapping(a, b)) {
                DrawRectangleLines(
                    bodies[i].position.x + bodies[i].box_shape.center.x - bodies[i].box_shape.extents.x,
                    bodies[i].position.y + bodies[i].box_shape.center.y - bodies[i].box_shape.extents.y,
                    2 * bodies[i].box_shape.extents.x,
                    2 * bodies[i].box_shape.extents.y,
                    RED
                );
                DrawRectangleLines(
                    bodies[j].position.x + bodies[j].box_shape.center.x - bodies[j].box_shape.extents.x,
                    bodies[j].position.y + bodies[j].box_shape.center.y - bodies[j].box_shape.extents.y,
                    2 * bodies[j].box_shape.extents.x,
                    2 * bodies[j].box_shape.extents.y,
                    RED
                );
            }
        }
    }
}

void rigidbodies_scene_cleanup(void) {
    free(vertex_buffer);
    arrfree(bodies);
    arrfree(collisions);
    UnloadTexture(background);
    // UnloadTexture(sphere_texture);
    // UnloadTexture(square_texture);
}

void rigidbodies_scene_load(PhysicScene *scene) {
    scene->init = rigidbodies_scene_init;
    scene->update = rigidbodies_scene_update;
    scene->render = rigidbodies_scene_render;
    scene->cleanup = rigidbodies_scene_cleanup;
}
